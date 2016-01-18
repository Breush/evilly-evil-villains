#include "scene/graph.hpp"

#include "nui/debug.hpp"
#include "core/application.hpp"
#include "scene/layer.hpp"
#include "tools/math.hpp"
#include "tools/event.hpp"
#include "tools/tools.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

using namespace scene;

Graph::Graph()
    : m_scene(this)
{
    // NUI layer
    m_nuiLayer.init(this);
    m_nuiLayer.setManipulable(false);
}

//-------------------//
//----- Routine -----//

void Graph::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Draw all layers
    target.draw(m_scene, states);
    target.draw(m_nuiLayer, states);

    // Drawing grabbed object
    // Note: Layer is now NUI
    if (m_grabbable != nullptr)
        target.draw(*m_grabbable, states);

    // Hovered child on debug
    debug_nui_1(drawMouseDetector(target, states));
}

void Graph::update(const sf::Time& dt, const float factor)
{
    // Grabbing
    if (m_grabbing)
        m_scene.moveGrabbing(m_grabbingPosition, dt);

    // Update recursively all the entities
    m_scene.update(dt, factor);
    m_nuiLayer.update(dt, factor);

    // The grabbable
    if (m_grabbable != nullptr)
        m_grabbable->update(dt);
}

void Graph::handleEvent(const sf::Event& event)
{
    // Window events are not considered
    returnif (isWindow(event));

    // Send event to the entity hovered/focused
    broadcastEvent(event);

    // Send event to all global handlers
    broadcastGlobalEvent(event);
}

void Graph::refreshWindow(const config::WindowInfo& cWindow)
{
    m_nuiLayer.setSize(cWindow.resolution);

    m_nuiLayer.refreshWindow(cWindow);
    m_scene.refreshWindow(cWindow);
}

void Graph::refreshNUI(const config::NUIGuides& cNUI)
{
    m_nuiLayer.refreshNUI(cNUI);
    m_scene.refreshNUI(cNUI);
}

//--------------------//
//----- Focusing -----//

void Graph::setFocusedEntity(Entity* focusedEntity)
{
    returnif (focusedEntity != nullptr && !focusedEntity->focusable());
    returnif (m_focusedEntity == focusedEntity);

    if (m_focusedEntity != nullptr)
        m_focusedEntity->setFocused(false);

    m_focusedEntity = focusedEntity;

    if (m_focusedEntity != nullptr)
        m_focusedEntity->setFocused(true);
}

void Graph::focusHandleEvent(const sf::Event& event)
{
    massert(m_focusedEntity != nullptr, "Internal logical error in focusing system.");

    // Just manage pressed keys
    returnif (event.type != sf::Event::KeyPressed);

    bool changeFocus = false;
    bool focusToNext = false;

    // Note: BackTab is not generated for every OS,
    // but in those where it is not generated, Tab is.
    if (event.key.code == sf::Keyboard::Tab) {
        changeFocus = true;
        focusToNext = !sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);
    }
    else if (event.key.code == sf::Keyboard::BackTab) {
        changeFocus = true;
        focusToNext = false;
    }
    else {
        focusToNext = (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::Down);
        changeFocus = focusToNext || (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Up);
    }

    returnif (!changeFocus);

    // Find next entity
    if (focusToNext) {
        Entity* nextFocused = m_focusedEntity->nextFocusable();
        if (nextFocused != nullptr) setFocusedEntity(nextFocused);
        else setFocusedEntity(m_nuiLayer.root().nextFocusable());
    }

    // Find previous entity
    else {
        Entity* previousFocused = m_focusedEntity->previousFocusable();
        if (previousFocused != nullptr) setFocusedEntity(previousFocused);
        else {
            previousFocused = m_nuiLayer.root().lastDescendant();
            if (previousFocused->focusable()) setFocusedEntity(previousFocused);
            else setFocusedEntity(previousFocused->previousFocusable());
        }
    }
}

//------------------//
//----- Events -----//

void Graph::broadcastEvent(const sf::Event& event)
{
    // Delegate for mouse
    if (isMouse(event)) {
        // Keep event if in grab mode
        if (event.type == sf::Event::MouseMoved)
            returnif (handleMouseMovedEvent(event));

        // Set focus on click
        bool entityKeptEvent = false;
        auto entity = handleMouseEvent(event, entityKeptEvent);
        if (entity != nullptr) {
            if (event.type == sf::Event::MouseButtonPressed)
                setFocusedEntity(entity);
            returnif (entityKeptEvent);
        }

        // Let this graph manage special events
        if (event.type == sf::Event::MouseWheelScrolled)
            handleMouseWheelScrolledEvent(event);
        else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Middle)
            handleMouseWheelPressedEvent(event);
        else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Middle)
            handleMouseWheelReleasedEvent(event);

        return;
    }

    // Keyboard or Joystick
    returnif (m_focusedEntity == nullptr);

    bool focusKept = false;
    if (isKeyboard(event)) focusKept = m_focusedEntity->handleKeyboardEvent(event);
    else focusKept = m_focusedEntity->handleJoystickEvent(event);
    returnif (focusKept);

    focusHandleEvent(event);
}

void Graph::broadcastGlobalEvent(const sf::Event& event)
{
    // FIXME There is a risk that some global handler will modify
    // the list of global handlers, causing some strange bugs.
    // A simple copy of the initial list might not work because of
    // pointers to unallowed.
    // One way to work around would be a for (i = 0; i < .size(); ++i)
    // so that is the size of the list increases, it's ok.
    // ---
    // I let it this way because the usage of global event is (and should be)
    // really rare. And all it does (in ScrollArea) are graphic updates, and never detach an entity.
    // So this case never happen so far.

    // Mouse event
    if (isMouse(event)) {
        sf::Vector2i mousePos(mousePosition(event));
        sf::Vector2f nuiPos(nuiPosition(mousePos));

        switch (event.type) {
        case sf::Event::MouseMoved:
            for (auto globalHandler : m_globalHandlers)
                globalHandler->handleGlobalMouseMoved(nuiPos);
            break;

        case sf::Event::MouseButtonPressed:
            for (auto globalHandler : m_globalHandlers)
                globalHandler->handleGlobalMouseButtonPressed(event.mouseButton.button, nuiPos);
            break;

        case sf::Event::MouseButtonReleased:
            for (auto globalHandler : m_globalHandlers)
                globalHandler->handleGlobalMouseButtonReleased(event.mouseButton.button, nuiPos);
            break;

        case sf::Event::MouseWheelScrolled:
            for (auto globalHandler : m_globalHandlers)
                globalHandler->handleGlobalMouseWheelScrolled(event.mouseWheelScroll.wheel, event.mouseWheelScroll.delta, nuiPos);
            break;

        default:
            break;
        }
    }

    // Keyboard event
    else if (isKeyboard(event)) {
        for (auto globalHandler : m_globalHandlers)
            globalHandler->handleGlobalKeyboardEvent(event);
    }

    // Joystick event
    else if (isJoystick(event)) {
        for (auto globalHandler : m_globalHandlers)
            globalHandler->handleGlobalJoystickEvent(event);
    }
}

void Graph::handleMouseWheelPressedEvent(const sf::Event& event)
{
    m_grabbingPosition = mousePosition(event);
    m_scene.startGrabbing(m_grabbingPosition);
    m_grabbing = true;
}

void Graph::handleMouseWheelReleasedEvent(const sf::Event& event)
{
    m_grabbing = false;
}

bool Graph::handleMouseMovedEvent(const sf::Event& event)
{
    m_grabbingPosition = mousePosition(event);
    return false;
}

void Graph::handleMouseWheelScrolledEvent(const sf::Event& event)
{
    float delta = event.mouseWheelScroll.delta;
    const auto& scrollingFactor = Application::context().display.global.scrollingFactor;

    // With Control pressed, zoom
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
        const auto& zoomSpeed = Application::context().display.global.zoomSpeed;
        float zoomFactor = 1.f + ((delta < 0.f)? zoomSpeed : -zoomSpeed);
        zoomFactor = std::pow(zoomFactor, std::abs(delta));
        m_scene.zoom(mousePosition(event), zoomFactor);
    }

    // With Shift pressed, move horizontally
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
        m_scene.moveView({-scrollingFactor * delta, 0.f});
    }

    // Else, move vertically
    else {
        m_scene.moveView({0.f, -scrollingFactor * delta});
    }
}

//----------------------------//
//----- Grabbing objects -----//

void Graph::grabbableHandleMouseEvent(const sf::Event& event)
{
    sf::Vector2i mousePos(mousePosition(event));
    sf::Vector2f nuiPos(nuiPosition(mousePos));
    sf::Vector2f viewPos, relPos;

    Entity* entity = entityFromPosition(mousePos, viewPos);

    if (entity != nullptr)
        relPos = entity->getInverseTransform().transformPoint(viewPos);

    switch (event.type) {
    case sf::Event::MouseMoved:
        m_grabbable->setLocalPosition(nuiPos);
        m_grabbable->spawner().grabbableMoved(entity, relPos, nuiPos);
        break;

    case sf::Event::MouseButtonPressed:
        m_grabbable->spawner().grabbableButtonPressed(entity, event.mouseButton.button, relPos, nuiPos);
        break;

    case sf::Event::MouseButtonReleased:
        m_grabbable->spawner().grabbableButtonReleased(entity, event.mouseButton.button, relPos, nuiPos);
        break;

    default:
        break;
    }
}

void Graph::setGrabbable(std::unique_ptr<Grabbable> grabbable)
{
    returnif (grabbable == nullptr);

    m_grabbable = std::move(grabbable);
    m_grabbable->setLocalPosition(nuiPosition());
}

void Graph::removeGrabbable()
{
    m_grabbable = nullptr;
}

//---------------------------//
//----- Mouse detection -----//

sf::Vector2f Graph::nuiPosition(sf::Vector2i mousePosition)
{
    const auto& window = Application::context().window;

    // Get current mouse position, if passed with no argument
    if (mousePosition.x < 0.f)
        mousePosition = sf::Mouse::getPosition(window);

    return window.mapPixelToCoords(mousePosition, m_nuiLayer.view());
}

void Graph::drawMouseDetector(sf::RenderTarget& target, sf::RenderStates states) const
{
    returnif (m_hoveredEntity == nullptr);

    sf::RectangleShape rectangleShape;
    rectangleShape.setFillColor({255, 0, 0, 150});
    rectangleShape.setSize(m_hoveredEntity->size());
    rectangleShape.setPosition(m_hoveredEntity->getPosition());
    rectangleShape.setRotation(m_hoveredEntity->getRotation());
    rectangleShape.setScale(m_hoveredEntity->getScale());
    rectangleShape.setOrigin(m_hoveredEntity->getOrigin());

    target.setView(viewFromLayerRoot(m_hoveredEntity->root()));
    target.draw(rectangleShape);
}

Entity* Graph::handleMouseEvent(const sf::Event& event, bool& entityKeptEvent)
{
    entityKeptEvent = false;

    // Let grabbable manage if any
    if (m_grabbable != nullptr) {
        grabbableHandleMouseEvent(event);
        return nullptr;
    }

    sf::Vector2i mousePos(mousePosition(event));
    sf::Vector2f nuiPos(nuiPosition(mousePos));
    sf::Vector2f viewPos;

    // Getting entity touched if any,
    // and setting viewPos to the position of mouse in the entity's layer's view
    Entity* entity = entityFromPosition(mousePos, viewPos);
    setHoveredEntity(entity);

    // Calling child callback
    while (entity != nullptr) {
        // Get the relative position of mouse in the entity
        sf::Vector2f relPos(entity->getInverseTransform().transformPoint(viewPos));

        switch (event.type) {
        case sf::Event::MouseMoved:
            entityKeptEvent = entity->handleMouseMoved(relPos, nuiPos);
            break;

        case sf::Event::MouseButtonPressed:
            entityKeptEvent = entity->handleMouseButtonPressed(event.mouseButton.button, relPos, nuiPos);
            break;

        case sf::Event::MouseButtonReleased:
            entityKeptEvent = entity->handleMouseButtonReleased(event.mouseButton.button, relPos, nuiPos);
            break;

        case sf::Event::MouseWheelScrolled:
            entityKeptEvent = entity->handleMouseWheelScrolled(event.mouseWheelScroll.wheel, event.mouseWheelScroll.delta, relPos, nuiPos);
            break;

        default:
            break;
        }

        // Try entity parent
        if (!entityKeptEvent) entity = entity->parent();
        else break;
    }

    // Entity could have been detached/destroyed,
    // and we just want to return the foremost entity.
    return entityFromPosition(mousePos, viewPos);
}

void Graph::setHoveredEntity(Entity* hoveredEntity)
{
    returnif (m_hoveredEntity == hoveredEntity);

    // Emit MouseLeft if mouse was over an other child
    if (m_hoveredEntity != nullptr)
        m_hoveredEntity->handleMouseLeft();

    m_hoveredEntity = hoveredEntity;

    debug_nui_2(if (m_hoveredEntity != nullptr));
    mdebug_nui_2("Hover: " << m_hoveredEntity->_name());
}

//---------------------------//
//---- Entity detection -----//

Entity* Graph::entityFromPosition(const sf::Vector2i& mousePos, sf::Vector2f& viewPos)
{
    // Check whether a detectable entity is at that position in NUI layer
    Entity* entity = m_nuiLayer.entityFromPosition(mousePos, viewPos);
    returnif (entity != nullptr) entity;

    // If not, check if there is one in scene layers
    return m_scene.entityFromPosition(mousePos, viewPos);
}

const sf::View& Graph::viewFromLayerRoot(const Entity* root) const
{
    if (&m_nuiLayer.root() == root)
        return m_nuiLayer.view();

    for (const auto& layer : m_scene.layers())
        if (&layer->root() == root)
            return layer->view();

    // Not found: problem
    throw std::runtime_error("Scene graph is inconsistent.");
}

//---------------------------//
//---- Entity callbacks -----//

void Graph::detachedEntity(Entity* entity)
{
    // Cancel focus if it was
    if (m_focusedEntity == entity)
        setFocusedEntity(nullptr);

    // Cancel hovering if it was
    if (m_hoveredEntity == entity)
        setHoveredEntity(nullptr);

    // Erase from the global handlers if it is one of them
    if (entity->handleGlobalEvents())
        std::erase_if(m_globalHandlers, [entity] (Entity* inEntity) { return inEntity == entity; });
}

void Graph::attachedEntity(Entity* entity)
{
    // Attribute focus if there is not yet
    if (m_focusedEntity == nullptr && entity->focusable())
        setFocusedEntity(entity);

    // Add to the global handlers if it is one of them
    if (entity->handleGlobalEvents())
        m_globalHandlers.emplace_back(entity);
}
