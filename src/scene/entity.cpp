#include "scene/entity.hpp"

#include "core/application.hpp"
#include "sfe/lerpable.hpp"
#include "scene/graph.hpp"
#include "tools/debug.hpp"
#include "tools/tools.hpp"
#include "tools/vector.hpp"
#include "tools/platform-fixes.hpp" // reverse

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/OpenGL.hpp>
#include <algorithm> // find_if

using namespace scene;

Entity::Entity(bool isLerpable)
    : m_depth(50.f)
    , m_relativePosition(0.f, 0.f)
    , m_relativeOrigin(0.f, 0.f)
    , m_localPosition(0.f, 0.f)
    , m_localRotation(0.f)
    , m_localScale(1.f, 1.f)
    , m_size(0.f, 0.f)
    , m_clipArea(0.f, 0.f, -1.f, -1.f)
    , m_visible(true)
    , m_transparent(false)
    , m_graph(nullptr)
    , m_parent(nullptr)
    , m_detectable(true)
    , m_focusable(false)
    , m_focusOwned(false)
{
    // Lerpable component
    if (isLerpable)
        m_lerpable = std::make_unique<sfe::Lerpable>(this);
}

Entity::~Entity()
{
    detachChildren();

    if (m_parent != nullptr)
         m_parent->detachChild(*this);
}

//-------------------//
//----- Routine -----//

void Entity::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    returnif (!m_visible);
    states.transform = getTransform();

    // Draw itself
    if (!m_transparent) {
        const auto initialShader = states.shader;
        if (m_shader != nullptr) states.shader = m_shader;

        // Sub-parts
        drawParts(target, states);

        // Draw focus shape
        if (m_focused) {
            states.shader = m_graph->focusShader();
            target.draw(m_graph->focusShape(), states);
        }

        states.shader = initialShader;
    }

    // Draw children - DFS
    for (auto& child : m_children)
        target.draw(*child, states);
}

void Entity::drawParts(sf::RenderTarget& target, sf::RenderStates states) const
{
    const auto& screenSize = Application::context().windowInfo.screenSize;
    const sf::Shader* initialShader = states.shader;
    sf::FloatRect glClipArea;

    // Clipping if needed
    if (m_globalClipping)
    {
        glEnable(GL_SCISSOR_TEST);

        glClipArea = m_globalClipArea;
        glClipArea = states.transform.transformRect(glClipArea);
        glClipArea = tools::mapRectCoordsToPixel(target, glClipArea);
        glClipArea.top = screenSize.y - glClipArea.height - glClipArea.top; // SFML - GL compatibility

        glScissor(glClipArea.left, glClipArea.top, glClipArea.width, glClipArea.height);
    }

    // Drawing parts
    for (auto& part : m_parts)
    {
        // Setting shader if needed
        if (part.shader != nullptr) states.shader = part.shader;
        else states.shader = initialShader;

        // Part clipping
        if (part.clipping) {
            // Part clipping rect
            sf::FloatRect r(part.clippingRect);
            r = states.transform.transformRect(r);
            r = tools::mapRectCoordsToPixel(target, r);
            r.top = screenSize.y - r.height - r.top;

            // If entity has already a clipping value, intersects
            if (m_globalClipping) r = tools::intersect(r, glClipArea);
            else glEnable(GL_SCISSOR_TEST);

            glScissor(r.left, r.top, r.width, r.height);
        }

        // Effectively drawing this part
        target.draw(*part.drawable, states);

        // Disable part clipping
        if (part.clipping && !m_globalClipping)
            glDisable(GL_SCISSOR_TEST);
    }

    // End of clipping
    if (m_globalClipping)
        glDisable(GL_SCISSOR_TEST);
}

void Entity::update(const sf::Time& dt)
{
    updateRoutine(dt);
    updateLerpable(dt);
    updateAI(dt);
    updateChanges();

    // Update children - DFS
    for (auto& child : m_children)
        child->update(dt);
}

void Entity::updateLerpable(const sf::Time& dt)
{
    if (m_lerpable != nullptr)
        m_lerpable->update(dt);
}

void Entity::updateChanges()
{
    // Delayed operations
    if (m_markedForVisible) {
        setVisible(m_visibleMark);
        m_markedForVisible = false;
    }

    // Update internal state if local changes
    if (m_localChanges) {
        if (m_focused)
            m_graph->updateFocusSprite();

        onTransformChanges();
        m_localChanges = false;
    }

    // Propagate to parent
    if (m_sizeChanges) {
        if (m_focused)
            m_graph->updateFocusSprite();

        if (m_parent != nullptr)
            m_parent->onChildSizeChanges(*this);

        m_sizeChanges = false;
    }
}

void Entity::refreshWindow(const config::WindowInfo& cWindow)
{
    for (auto& child : m_children)
        child->refreshWindow(cWindow);
}

void Entity::refreshNUI(const config::NUIGuides& cNUI)
{
    for (auto& child : m_children)
        child->refreshNUI(cNUI);
}

void Entity::refresh()
{
    refreshWindow(Application::context().windowInfo);
    refreshNUI(Application::context().nuiGuides);
}

//-------------------------------//
//----- Children management -----//

bool Entity::hasChild(const Entity& entity) const
{
    auto found = std::find_if(m_children, [&entity](const Entity* node) { return node == &entity; });
    return found != std::end(m_children);
}

void Entity::attachChild(Entity& child)
{
    massert(!hasChild(child), "Trying to attach an already attached entity.");

    child.setGraph(m_graph);
    child.setParent(this);
    m_children.emplace_back(&child);
    refreshChildrenOrder();
}

void Entity::detachChild(Entity& child)
{
    auto found = std::find_if(m_children, [&child](const Entity* node) { return node == &child; });
    massert(found != std::end(m_children), "Could not detach child.");

    onChildDetached(child);
    child.setParent(nullptr);
    child.setGraph(nullptr);
    m_children.erase(found);
}

void Entity::detachChildren()
{
    for (auto& child : m_children) {
        onChildDetached(*child);
        child->setParent(nullptr);
        child->setGraph(nullptr);
    }

    m_children.clear();
}

Entity* Entity::firstOver(const sf::Vector2f& position)
{
    // Reversed-DFS search for first children over position
    for (const auto& child : std::reverse(m_children)) {
        Entity* entity = child->firstOver(position);
        returnif (entity != nullptr) entity;
    }

    // Children are not over, maybe I am
    // Note: transparency does not affect detectability
    returnif (!m_detectable || !m_visible) nullptr;
    sf::FloatRect localBounds({0.f, 0.f}, size());
    localBounds = tools::intersect(m_globalClipArea, localBounds);
    returnif (localBounds.width < 0.f || localBounds.height < 0.f) nullptr;
    returnif (localBounds.contains(getInverseTransform().transformPoint(position))) this;

    return nullptr;
}

//---------------------------//
//----- Focusing system -----//

void Entity::setFocusRect(const sf::FloatRect& inFocusRect)
{
    m_focusRect = inFocusRect;
    if (m_focused && m_graph != nullptr) m_graph->updateFocusSprite();
}

void Entity::setFocused(bool inFocused)
{
    m_focused = inFocused;
    if (m_focused && m_graph != nullptr) m_graph->updateFocusSprite();
}

Entity* Entity::closestNextSibling()
{
    returnif (m_parent == nullptr) nullptr;

    bool foundSelf = false;
    for (auto& sibling : m_parent->children()) {
        // Skip previous
        if (!foundSelf && sibling != this) continue;
        if (sibling == this) foundSelf = true;
        else return sibling;
    }

    return m_parent->closestNextSibling();
}

Entity* Entity::nextFocusableChild()
{
    for (auto& child : m_children) {
        returnif (child->focusable()) child;
        Entity* nextFocusedChild = child->nextFocusableChild();
        returnif (nextFocusedChild != nullptr) nextFocusedChild;
    }

    return nullptr;
}

Entity* Entity::nextFocusable()
{
    // Check if there is a focusable child
    Entity* nextFocusedChild = nextFocusableChild();
    returnif (nextFocusedChild != nullptr) nextFocusedChild;

    // Look through siblings
    Entity* sibling = closestNextSibling();
    returnif (sibling == nullptr) nullptr;
    returnif (sibling->focusable()) sibling;
    return sibling->nextFocusable();
}

Entity* Entity::lastDescendant()
{
    returnif (m_children.size() == 0) this;
    return m_children.back()->lastDescendant();
}

Entity* Entity::closestPreviousSibling()
{
    returnif (m_parent == nullptr) nullptr;

    bool foundSelf = false;
    for (const auto& sibling : std::reverse(m_parent->children())) {
        // Skip next
        if (!foundSelf && sibling != this) continue;
        if (sibling == this) foundSelf = true;
        else return sibling->lastDescendant();
    }

    // No previous sibling
    return m_parent;
}

Entity* Entity::previousFocusable()
{
    // Check if we came from the closest previous sibling
    Entity* closestPreviousEntity = closestPreviousSibling();
    returnif (closestPreviousEntity == nullptr) nullptr;
    returnif (closestPreviousEntity->focusable()) closestPreviousEntity;

    // If not, then go recursive
    return closestPreviousEntity->previousFocusable();
}

//-----------------//
//----- Parts -----//

void Entity::clearParts()
{
    m_parts.clear();
}

void Entity::addPart(sf::Drawable* drawable)
{
    // Do not re-add part
    for (auto& part : m_parts) {
        if (part.drawable == drawable) {
            throw std::logic_error("Trying to add a part that was already added.");
            return;
        }
    }

    m_parts.push_back({drawable, nullptr, false});
}

void Entity::removePart(sf::Drawable* drawable)
{
    for (auto it = m_parts.begin(); it != m_parts.end(); ++it) {
        if (it->drawable == drawable) {
            m_parts.erase(it);
            return;
        }
    }

    throw std::logic_error("Trying to remove a part that was not previously added.");
}

void Entity::setPartShader(sf::Drawable* drawable, const std::string& shaderID)
{
    returnif (!sf::Shader::isAvailable());

    for (auto& part : m_parts) {
        if (part.drawable == drawable) {
            part.shader = &Application::context().shaders.get(shaderID);
            return;
        }
    }
}

void Entity::setPartClippingRect(sf::Drawable* drawable, const sf::FloatRect& clippingRect)
{
    for (auto& part : m_parts) {
        if (part.drawable == drawable) {
            part.clipping = true;
            part.clippingRect = clippingRect;
            return;
        }
    }
}

void Entity::resetPartShader(sf::Drawable* drawable)
{
    for (auto& part : m_parts) {
        if (part.drawable == drawable) {
            part.shader = nullptr;
            return;
        }
    }
}

void Entity::resetPartsShader()
{
    for (auto& part : m_parts)
        part.shader = nullptr;
}

//-------------------------//
//----- Local actions -----//

void Entity::setRelativePosition(const sf::Vector2f& inRelativePosition)
{
    m_relativePositioning = true;
    m_relativePosition = inRelativePosition;
    refreshRelativePosition();
}

void Entity::centerOrigin()
{
    setRelativeOrigin({0.5f, 0.5f});
}

sf::FloatRect Entity::localBounds()
{
    return {m_localPosition.x - getOrigin().x, m_localPosition.y - getOrigin().y, m_size.x, m_size.y};
}

void Entity::setSize(const sf::Vector2f& inSize)
{
    m_size = inSize;
    m_sizeChanges = true;

    if (!m_focusOwned)
        setFocusRect({0.f, 0.f, m_size.x, m_size.y});

    refreshChildrenRelativePosition();
    refreshOrigin();
    onSizeChanges();
}

void Entity::setLocalPosition(const sf::Vector2f& inLocalPosition)
{
    m_localPosition = inLocalPosition;

    refreshKeepInsideLocalRect();
    refreshFromLocalPosition();
}

void Entity::setShader(const std::string& shaderID)
{
    if (shaderID.empty()) m_shader = nullptr;
    else m_shader = &Application::context().shaders.get(shaderID);
}

//------------------------------------//
//----- Refresh on local changes -----//

void Entity::refreshRelativePosition()
{
    returnif (!m_relativePositioning || m_parent == nullptr);
    setLocalPosition(m_relativePosition * m_parent->size());
}

void Entity::refreshFromLocal()
{
    m_localChanges = true;

    if (m_parent != nullptr) {
        setPosition(m_parent->getPosition() - m_parent->getOrigin());
        setRotation(m_parent->getRotation());
        setScale(m_parent->getScale());
        move(m_localPosition);
        rotate(m_localRotation);
        scale(m_localScale);
    }
    else {
        setPosition(m_localPosition);
        setRotation(m_localRotation);
        setScale(m_localScale);
    }

    if (m_focused) m_graph->updateFocusSprite();

    for (auto& child : m_children)
        child->refreshFromLocal();
}

void Entity::refreshFromLocalPosition()
{
    m_localChanges = true;

    if (m_parent != nullptr) {
        setPosition(m_parent->getPosition() - m_parent->getOrigin());
        move(m_localPosition);
    }
    else {
        setPosition(m_localPosition);
    }

    if (m_focused) m_graph->updateFocusSprite();

    for (auto& child : m_children)
        child->refreshFromLocalPosition();
}

void Entity::refreshFromLocalRotation()
{
    m_localChanges = true;

    if (m_parent != nullptr) {
        setRotation(m_parent->getRotation());
        rotate(m_localRotation);
    }
    else {
        setRotation(m_localRotation);
    }

    if (m_focused) m_graph->updateFocusSprite();

    for (auto& child : m_children)
        child->refreshFromLocalRotation();
}

void Entity::refreshFromLocalScale()
{
    m_localChanges = true;

    if (m_parent != nullptr) {
        setScale(m_parent->getScale());
        scale(m_localScale);
    }
    else {
        setScale(m_localScale);
    }

    if (m_focused) m_graph->updateFocusSprite();

    for (auto& child : m_children)
        child->refreshFromLocalScale();

    onScaleChanges();
}

void Entity::refreshOrigin()
{
    setOrigin(m_relativeOrigin * m_size);
    refreshKeepInsideLocalRect();
    refreshFromLocalPosition();
}

void Entity::refreshDepthOrder()
{
    if (m_parent != nullptr)
        m_parent->refreshChildrenOrder();
}

void Entity::refreshChildrenOrder()
{
    m_children.sort([](Entity* a, Entity* b) { return a->depth() > b->depth(); });
}

void Entity::refreshChildrenRelativePosition()
{
    for (auto& child : m_children)
        child->refreshRelativePosition();
}

void Entity::refreshClipArea()
{
    m_globalClipArea = m_clipArea;

    if (m_parent != nullptr) {
        auto position = m_localPosition - getOrigin();
        m_globalClipArea = tools::intersect(m_globalClipArea, m_parent->globalClipArea() - position);
    }

    m_globalClipping = (m_globalClipArea.width >= 0.f && m_globalClipArea.height >= 0.f);

    for (auto& child : m_children)
        child->refreshClipArea();
}

bool Entity::refreshKeepInsideLocalRect()
{
    bool moved = false;

    // Horizontal
    if (m_insideLocalRect.width > 0.f) {
        // West
        auto xw = m_localPosition.x - getOrigin().x;
        if (xw < m_insideLocalRect.left) {
            m_localPosition.x += m_insideLocalRect.left - xw;
            moved = true;
        }

        // East
        auto xe = m_localPosition.x + m_size.x - getOrigin().x;
        if (xe > m_insideLocalRect.left + m_insideLocalRect.width) {
            m_localPosition.x += m_insideLocalRect.left + m_insideLocalRect.width - xe;
            moved = true;
        }
    }

    // Vertical
    if (m_insideLocalRect.height > 0.f) {
        // North
        auto yn = m_localPosition.y - getOrigin().y;
        if (yn < m_insideLocalRect.top) {
            m_localPosition.y += m_insideLocalRect.top - yn;
            moved = true;
        }

        // South
        auto ys = m_localPosition.y + m_size.y - getOrigin().y;
        if (ys > m_insideLocalRect.top + m_insideLocalRect.height) {
            m_localPosition.y += m_insideLocalRect.top + m_insideLocalRect.height - ys;
            moved = true;
        }
    }

    return moved;
}

//---------------------//
//----- Utilities -----//

void Entity::setKeepInsideLocalRect(const sf::FloatRect& localRect)
{
    returnif (m_insideLocalRect == localRect);
    m_insideLocalRect = localRect;
    refreshKeepInsideLocalRect();
    refreshFromLocalPosition();
}

void Entity::markForVisible(bool visible)
{
    m_markedForVisible = true;
    m_visibleMark = visible;
}

void Entity::giveFocus()
{
    if (m_focusable && m_graph != nullptr)
        m_graph->setFocusedEntity(this);
}

void Entity::setParent(Entity* inParent)
{
    // If it has no parent before,
    // a refresh of NUI/Window is forced.
    if (m_parent == nullptr && inParent != nullptr)
        refresh();

    // Affect new parent
    m_parent = inParent;
    refreshFromLocal();
    refreshRelativePosition();
    refreshClipArea();
}

//-----------------//
//----- Graph -----//

void Entity::setGraph(Graph* inGraph)
{
    if (m_graph != nullptr)
        m_graph->detachedEntity(this);

    m_graph = inGraph;
    for (auto& child : m_children)
        child->setGraph(inGraph);

    if (m_graph != nullptr)
        m_graph->attachedEntity(this);
}

Entity* Entity::root()
{
    returnif (m_parent == nullptr) this;
    return m_parent->root();
}
