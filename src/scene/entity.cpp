#include "scene/entity.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
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
    , m_localPosition(0.f, 0.f)
    , m_localRotation(0.f)
    , m_localScale(1.f, 1.f)
    , m_size(0.f, 0.f)
    , m_centered(false)
    , m_visible(true)
    , m_graph(nullptr)
    , m_parent(nullptr)
    , m_detectable(true)
    , m_focusable(false)
    , m_focusOwned(false)
{
    // Lerpable component
    if (isLerpable)
        m_lerpable = std::unique_ptr<sfe::Lerpable>(new sfe::Lerpable(this));
}

//-------------------//
//----- Routine -----//

void Entity::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform = getTransform();

    // Draw itself
    if (m_visible) {
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
    // TODO [OPTIM] Do not recompute each time...
    const auto& screenSize = Application::context().screenSize;
    const auto& resolution = Application::context().resolution;
    const auto& effectiveDisplay = Application::context().effectiveDisplay;

    auto halfGap = (screenSize - effectiveDisplay) / 2.f;
    auto viewRatio = effectiveDisplay / resolution;

    const sf::Shader* initialShader = states.shader;

    // Drawing parts
    for (auto& part : m_parts)
    {
        // Setting shader if needed
        if (part.shader != nullptr) states.shader = part.shader;
        else states.shader = initialShader;

        // Clipping if needed
        if (part.clipping) {
            glEnable(GL_SCISSOR_TEST);

            sf::FloatRect r(part.clippingRect);
            r = states.transform.transformRect(r);

            // Correct position and ratio SFML <-> GLSL
            r.top = resolution.y - r.height - r.top;
            r *= viewRatio;
            r += halfGap;

            glScissor(r.left, r.top, r.width, r.height);
        }

        // Effectively drawing this part
        target.draw(*part.drawable, states);

        // End of clipping
        if (part.clipping)
            glDisable(GL_SCISSOR_TEST);
    }
}

void Entity::update(const sf::Time& dt)
{
    updateRoutine(dt);
    updateLerpable(dt);
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

    // Propagate to children
    if (m_localChanges) {
        if (m_focused)
            m_graph->updateFocusSprite();

        if (m_parent != nullptr && m_callParentOnLocalChanges)
            m_parent->update();

        m_localChanges = false;
    }

    if (m_sizeChanges) {
        if (m_focused)
            m_graph->updateFocusSprite();

        if (m_parent != nullptr && m_callParentOnSizeChanges)
            m_parent->update();

        m_sizeChanges = false;
    }
}

void Entity::refreshDisplay()
{
    for (auto& child : m_children)
        child->refreshDisplay();
}

//-------------------------------//
//----- Children management -----//

void Entity::attachChild(Entity& child)
{
    child.setGraph(m_graph);
    child.setParent(this);
    m_children.emplace_back(&child);
    refreshChildrenOrder();
}

void Entity::detachChild(Entity& child)
{
    auto found = std::find_if(m_children.begin(), m_children.end(), [&](Entity* node) { return node == &child; });
    massert(found != m_children.end(), "Could not detach child.");

    child.setParent(nullptr);
    child.setGraph(nullptr);
    m_children.erase(found);
}

Entity* Entity::firstOver(const sf::Vector2f& position)
{
    // Reversed-DFS search for first children over position
    for (const auto& child : std::reverse(m_children)) {
        Entity* entity = child->firstOver(position);
        returnif (entity != nullptr) entity;
    }

    // Children are not over, maybe I am
    returnif (!detectable() || !visible()) nullptr;
    sf::FloatRect localBounds({0.f, 0.f}, size());
    returnif (localBounds.contains(getInverseTransform().transformPoint(position))) this;

    return nullptr;
}

//---------------------------//
//----- Focusing system -----//

void Entity::setFocusRect(const sf::FloatRect& inFocusRect)
{
    m_focusRect = inFocusRect;
    if (m_focused) m_graph->updateFocusSprite();
}

void Entity::setFocused(bool inFocused)
{
    m_focused = inFocused;
    if (m_focused) m_graph->updateFocusSprite();
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
    for (auto& part : m_parts)
        returnif (part.drawable == drawable);

    m_parts.push_back({drawable, nullptr, false});
}

void Entity::removePart(sf::Drawable* drawable)
{
    for (uint i = 0; i < m_parts.size(); ++i) {
        if (m_parts[i].drawable == drawable) {
            m_parts.erase(m_parts.begin() + i);
            return;
        }
    }
}

void Entity::setPartShader(sf::Drawable* drawable, ShaderID shaderID)
{
    returnif (!sf::Shader::isAvailable());

    for (auto& part : m_parts)
    {
        if (part.drawable == drawable)
        {
            part.shader = &Application::context().shaders.get(shaderID);
            return;
        }
    }
}

void Entity::setPartClippingRect(sf::Drawable* drawable, const sf::FloatRect& clippingRect)
{
    for (auto& part : m_parts)
    {
        if (part.drawable == drawable)
        {
            part.clipping = true;
            part.clippingRect = clippingRect;
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
    m_relativePositionning = true;
    m_relativePosition = inRelativePosition;
    refreshRelativePosition();
}

void Entity::setSize(const sf::Vector2f& resize)
{
    m_size = resize;
    m_sizeChanges = true;

    if (!m_focusOwned)
        setFocusRect({0.f, 0.f, m_size.x, m_size.y});

    refreshChildrenRelativePosition();
    refreshCentering();
    update();
}

void Entity::setShader(ShaderID shaderID)
{
    if (shaderID == ShaderID::NONE) m_shader = nullptr;
    else m_shader = &Application::context().shaders.get(shaderID);
}

//------------------------------------//
//----- Refresh on local changes -----//

void Entity::refreshRelativePosition()
{
    returnif (!m_relativePositionning || m_parent == nullptr);
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
}

void Entity::refreshCentering()
{
    if (m_centered) setOrigin(0.5f * size());
    else setOrigin(0.f, 0.f);
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

//---------------------//
//----- Utilities -----//

void Entity::markForVisible(bool visible)
{
    m_markedForVisible = true;
    m_visibleMark = visible;
}

void Entity::setParent(Entity* inParent)
{
    m_parent = inParent;
    refreshFromLocal();
    refreshRelativePosition();
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
