#include "scene/entity.hpp"

#include "context/context.hpp"
#include "scene/components/component.hpp"
#include "scene/graph.hpp"
#include "tools/debug.hpp"
#include "tools/tools.hpp"
#include "tools/vector.hpp"
#include "tools/platform-fixes.hpp" // reverse

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/OpenGL.hpp>
#include <algorithm> // find_if

using namespace scene;

Entity::Entity()
    : m_depth(50.f)
    , m_relativePosition(0.f, 0.f)
    , m_relativeOrigin(0.f, 0.f)
    , m_localPosition(0.f, 0.f)
    , m_localRotation(0.f)
    , m_localScale(1.f, 1.f)
    , m_size(0.f, 0.f)
    , m_scale(1.f, 1.f)
    , m_visible(true)
    , m_transparent(false)
    , m_graph(nullptr)
    , m_parent(nullptr)
    , m_detectable(true)
    , m_focusable(false)
    , m_focusOwned(false)
{
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

    // Check if this entity is not visible because completely out
    // of the view rect. If so, no need for drawing itself and its children.
    // OPTIM: do not recompute that each time
    /*sf::FloatRect viewRect{target.getView().getCenter() - target.getView().getSize() / 2.f, target.getView().getSize()};
    viewRect = tools::intersect(viewRect, localBounds() + getPosition() - getOrigin());
    returnif (viewRect.width < 0.f || viewRect.height < 0.f);*/

    states.transform = getTransform();
    states.shader = m_shader;

    // No personnal global clipping
    // Note: if any, the previous clipping area will still be applied
    if (m_globalClipAreas.empty())
    {
        // Draw self
        if (!m_transparent) {
            drawParts(target, states);
            drawInternal(target, states);
        }

        // Draw children - DFS
        for (auto& child : m_children)
            child->draw(target, states);

        return;
    }

    // Was there a previous clipping?
    const auto& parentClippingAreaInt = target.getClippingArea();
    auto parentClippingArea = sf::toFloatRect(parentClippingAreaInt);
    bool parentClipping = (parentClippingAreaInt != sf::IntRect());

    // Multiple global clipping
    for (const auto& globalClipArea : m_globalClipAreas)
    {
        sf::FloatRect clipArea;
        clipArea = tools::mapRectCoordsToPixel(target, globalClipArea);
        if (parentClipping) clipArea = tools::intersect(parentClippingArea, clipArea);

        // Fully clipped
        // TODO Add a pre-clipping test: does it worth it to draw itself?
        // i.e. Are we invisible because of the clip area?
        if (clipArea.width <= 0.f || clipArea.height <= 0.f) continue;

        target.setClippingArea(sf::IntRect(clipArea.left, clipArea.top, clipArea.width, clipArea.height));

        // Draw self
        if (!m_transparent) {
            drawParts(target, states, clipArea);
            drawInternal(target, states);
        }

        // Draw children - DFS
        for (auto& child : m_children)
            child->draw(target, states);
    }

    // Reset previous clipping
    // Note: If there was no clipping, the setClippingArea(sf::IntRect())
    // is the behavior of clearClippingArea()
    target.setClippingArea(parentClippingAreaInt);
}

void Entity::drawParts(sf::RenderTarget& target, sf::RenderStates states) const
{
    const sf::Shader* initialShader = states.shader;

    // Drawing parts
    for (auto& part : m_parts)
    {
        // Setting shader if needed
        if (part.shader != nullptr)
            states.shader = part.shader;

        // Part clipping
        if (part.clipping) {
            sf::FloatRect partClipArea(part.clippingRect);
            partClipArea = states.transform.transformRect(partClipArea);
            partClipArea = tools::mapRectCoordsToPixel(target, partClipArea);

            if (partClipArea.width <= 0.f || partClipArea.height <= 0.f) continue;
            target.setClippingArea(sf::IntRect(partClipArea.left, partClipArea.top, partClipArea.width, partClipArea.height));
        }

        // Effectively drawing this part
        target.draw(*part.drawable, states);

        // Restore the previous clipping
        if (part.clipping)
            target.clearClippingArea();

        // Reset the shader
        if (part.shader != nullptr)
            states.shader = initialShader;
    }
}

void Entity::drawParts(sf::RenderTarget& target, sf::RenderStates states, const sf::FloatRect& clipArea) const
{
    const sf::Shader* initialShader = states.shader;

    // Drawing parts
    for (auto& part : m_parts)
    {
        // Setting shader if needed
        if (part.shader != nullptr)
            states.shader = part.shader;

        // Part clipping
        if (part.clipping) {
            sf::FloatRect partClipArea(part.clippingRect);
            partClipArea = states.transform.transformRect(partClipArea);
            partClipArea = tools::mapRectCoordsToPixel(target, partClipArea);
            partClipArea = tools::intersect(partClipArea, clipArea);

            if (partClipArea.width <= 0.f || partClipArea.height <= 0.f) continue;
            target.setClippingArea(sf::IntRect(partClipArea.left, partClipArea.top, partClipArea.width, partClipArea.height));
        }

        // Effectively drawing this part
        target.draw(*part.drawable, states);

        // Restore the previous clipping
        if (part.clipping)
            target.setClippingArea(sf::IntRect(clipArea.left, clipArea.top, clipArea.width, clipArea.height));

        // Reset the shader
        if (part.shader != nullptr)
            states.shader = initialShader;
    }
}

void Entity::update(const sf::Time& dt, const float factor)
{
    auto dtGame = dt * factor;

    updateRoutine(dtGame);
    updateAI(dtGame);
    updateChanges();

    // Update children - DFS
    for (auto& child : m_children)
        child->update(dt, factor);
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
        refreshClipAreas();

        onTransformChanges();

        // Warn components
        for (auto& component : m_components)
            reinterpret_cast<scene::Component*>(component.second)->onTransformChanged();

        m_localChanges = false;
    }

    // Size has changed
    if (m_sizeChanges) {
        // Propagate to parent
        if (m_parent != nullptr)
            m_parent->onChildSizeChanges(*this);

        // Note: we do not put onSizeChanges() here
        // because it is ensured that the size changes are in sync.
        m_sizeChanges = false;
    }

    // Update focus sprite
    if (m_focused && (m_localChanges || m_sizeChanges))
        onFocusChanged();
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
    refreshWindow(context::context.windowInfo);
    refreshNUI(context::context.nuiGuides);
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

    child.setLayer(m_layer);
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
    child.setLayer(nullptr);
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
    // Invisible, so are the children
    // Note: transparency does not affect detectability
    returnif (!m_visible) nullptr;

    // Reversed-DFS search for first children over position
    for (const auto& child : std::reverse(m_children)) {
        Entity* entity = child->firstOver(position);
        returnif (entity != nullptr) entity;
    }

    // Children are not over, maybe I am
    returnif (!m_detectable) nullptr;

    // Gross check to see if we are hitting this entity
    auto relPos = getInverseTransform().transformPoint(position);
    returnif (relPos.x < 0.f || relPos.y < 0.f || relPos.x >= m_size.x || relPos.y >= m_size.y) nullptr;

    // Is the point really visible?
    returnif (!isPointInClipAreas(position)) nullptr;
    returnif (!isPointOverable(relPos)) nullptr;

    return this;
}

//---------------------------//
//----- Focusing system -----//

void Entity::setFocusRect(const sf::FloatRect& inFocusRect)
{
    m_focusRect = inFocusRect;
    if (m_focused && m_graph != nullptr)
        onFocusChanged();
}

void Entity::setFocused(bool inFocused)
{
    m_focused = inFocused;
    if (m_focused && m_graph != nullptr)
        onFocusChanged();
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
        if (part.drawable == drawable)
            throw std::logic_error("Trying to add a part that was already added.");

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
            part.shader = shaderID.empty()? nullptr : &context::context.shaders.get(shaderID);
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

sf::FloatRect Entity::localBounds() const
{
    return {0.f, 0.f, m_size.x, m_size.y};
}

sf::FloatRect Entity::globalBounds()
{
    return getInverseTransform().transformRect(localBounds());
}

void Entity::setSize(const sf::Vector2f& inSize)
{
    returnif (m_size == inSize);

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
    returnif (!sf::Shader::isAvailable());

    if (shaderID.empty()) m_shader = nullptr;
    else m_shader = &context::context.shaders.get(shaderID);
}

//--------------------//
//----- Clipping -----//

void Entity::setClipArea(const sf::FloatRect& clipArea)
{
    resetClipAreas();
    addClipArea(clipArea);
}

void Entity::resetClipAreas()
{
    m_clipAreas.clear();
    m_globalClipAreas.clear();
}

void Entity::addClipArea(const sf::FloatRect& clipArea, bool absolute)
{
    returnif (clipArea.width < 0.f || clipArea.height < 0.f);

    m_clipAreas.emplace_back(clipArea, absolute);
    m_globalClipAreas.emplace_back(clipArea);

    refreshClipArea(m_clipAreas.size() - 1u);
}

bool Entity::isPointInClipAreas(const sf::Vector2f& position)
{
    returnif (m_globalClipAreas.empty()) true;

    for (const auto& globalClipArea : m_globalClipAreas)
        returnif (globalClipArea.contains(position)) true;

    return false;
}

//! @}

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
        m_scale = m_parent->scale();
        move(m_localPosition);
        rotate(m_localRotation);
        m_scale *= m_localScale;
    }
    else {
        setPosition(m_localPosition);
        setRotation(m_localRotation);
        m_scale = m_localScale;
    }

    for (auto& child : m_children)
        child->refreshFromLocal();

    onRotationChanges();
    onScaleChanges();
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

    for (auto& child : m_children)
        child->refreshFromLocalRotation();

    onRotationChanges();
}

void Entity::refreshFromLocalScale()
{
    m_localChanges = true;

    if (m_parent != nullptr) {
        m_scale = m_parent->scale();
        m_scale *= m_localScale;
    } else {
        m_scale = m_localScale;
    }

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

void Entity::refreshClipArea(const uint index)
{
    returnif (m_clipAreas[index].absolute);
    m_globalClipAreas[index] = getTransform().transformRect(m_clipAreas[index].area);
}

void Entity::refreshClipAreas()
{
    auto clipAreasCount = m_clipAreas.size();
    for (uint i = 0u; i < clipAreasCount; ++i)
        refreshClipArea(i);
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
    returnif (m_parent == inParent);

    // If it has no parent before,
    // a refresh of NUI/Window is forced.
    if (m_parent == nullptr && inParent != nullptr)
        refresh();

    // Affect new parent
    m_parent = inParent;

    // Refresh on attach
    if (m_parent != nullptr) {
        refreshFromLocal();
        refreshRelativePosition();
        refreshClipAreas();
    }
}

//-----------------//
//----- Graph -----//

void Entity::setLayer(Layer* inLayer)
{
    returnif (m_layer == inLayer);

    m_layer = inLayer;
    for (auto& child : m_children)
        child->setLayer(m_layer);

    // Warn components
    for (auto& component : m_components)
        reinterpret_cast<scene::Component*>(component.second)->onLayerChanged(m_layer);
}

void Entity::setGraph(Graph* inGraph)
{
    if (m_graph != nullptr)
        m_graph->detachedEntity(this);

    m_graph = inGraph;
    for (auto& child : m_children)
        child->setGraph(m_graph);

    if (m_graph != nullptr)
        m_graph->attachedEntity(this);
}

Entity* Entity::root()
{
    returnif (m_parent == nullptr) this;
    return m_parent->root();
}
