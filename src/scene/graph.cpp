#include "scene/graph.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "tools/math.hpp"
#include "tools/event.hpp"
#include "tools/tools.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

using namespace scene;

Graph::Graph()
{
    // Focusing
    m_focusShader = &Application::context().shaders.get(ShaderID::NUI_FOCUS);
    m_focusShape.setTexture(&Application::context().textures.get(TextureID::NUI_FOCUS));
    m_focusShape.setFillColor({255, 255, 255, 100});

    // Default parameters
    for (auto& layer : m_layers) {
        layer.root.setDetectable(false);
        layer.root.setFocusable(false);
        layer.root.setVisible(false);
        layer.root.setGraph(this);
    }

    // Initialisation of layers views
    m_layers[LayerID::DUNGEON_DESIGN].view = &Application::context().views.get(ViewID::DUNGEON_DESIGN);
    m_layers[LayerID::DUNGEON_DESIGN].manipulable = true;

    m_layers[LayerID::NUI].view = &Application::context().views.get(ViewID::NUI);
    m_layers[LayerID::NUI].manipulable = false;
}

const sf::View& Graph::viewFromLayerRoot(const Entity* root) const
{
    for (const auto& layer : m_layers)
        if (&layer.root == root)
            return *layer.view;

    // Not found: problem
    throw std::runtime_error("Scene graph is inconsistent.");
}

//-------------------//
//----- Routine -----//

void Graph::update(const sf::Time& dt)
{
    // Update recursively all the entities
    for (auto& layerRootEntity : m_layers)
        layerRootEntity.root.update(dt);

    // Focusing system - animation
    if (m_focusedEntity != nullptr) {
        m_focusAnimation += 60.f * dt.asSeconds();
        const auto& focusSize = m_focusShape.getSize();
        m_focusShape.setTextureRect(sf::IntRect(-m_focusAnimation, -m_focusAnimation, focusSize.x, focusSize.y));
    }
}

void Graph::handleEvent(const sf::Event& event)
{
    // Window events are not considered
    returnif (isWindow(event));

    // Delegate for mouse
    if (isMouse(event)) {
        // Mouse wheel
        if (event.type == sf::Event::MouseWheelMoved) {
            handleMouseWheelMovedEvent(event);
            return;
        }
        else if (event.type == sf::Event::MouseButtonPressed
                && event.mouseButton.button == sf::Mouse::Button::Middle) {
            handleMouseWheelPressedEvent(event);
            return;
        }
        else if (event.type == sf::Event::MouseButtonReleased
                && event.mouseButton.button == sf::Mouse::Button::Middle) {
            handleMouseWheelReleasedEvent(event);
            return;
        }
        else if (event.type == sf::Event::MouseMoved) {
            returnif (handleMouseMovedEvent(event));
        }

        // Set focus on click
        auto entity = handleMouseEvent(event);
        if (entity != nullptr && event.type == sf::Event::MouseButtonPressed)
            setFocusedEntity(entity);

        return;
    }

    // Keyboard or Joystick
    returnif (m_focusedEntity == nullptr);

    if (isKeyboard(event)) m_focusedEntity->handleKeyboardEvent(event);
    else m_focusedEntity->handleJoystickEvent(event);

    // TODO Have a grabbed focus feedback
    focusHandleEvent(event);
}

void Graph::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Draw all layers
    for (auto& layer : m_layers) {
        target.setView(*layer.view);
        target.draw(layer.root, states);
    }

    // Drawing grabbed object
    // Note: Layer is now NUI
    if (m_grabbable.get() != nullptr)
        target.draw(*m_grabbable, states);

    // Hovered child on debug
    debug_nui_1(drawMouseDetector(target, states));
}

//--------------------//
//----- Focusing -----//

void Graph::updateFocusSprite()
{
    returnif (m_focusedEntity == nullptr);
    const sf::FloatRect& focusRect = m_focusedEntity->focusRect();

    sf::Vector2f focusPosition(focusRect.left, focusRect.top);
    sf::Vector2f focusSize(focusRect.width, focusRect.height);

    m_focusShape.setPosition(focusPosition);
    m_focusShape.setSize(focusSize);

    sf::Vector2f globalFocusPosition = m_focusedEntity->getPosition() - m_focusedEntity->getOrigin() + focusPosition;

    Application::context().shaders.setParameter(ShaderID::NUI_FOCUS, "position", globalFocusPosition);
    Application::context().shaders.setParameter(ShaderID::NUI_FOCUS, "textureSize", focusSize);
}

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

    // Find next entity
    if (event.key.code == sf::Keyboard::Right
        || event.key.code == sf::Keyboard::Down
        || event.key.code == sf::Keyboard::Tab) {
        // Simply get next or restart from the beginning
        Entity* nextFocused = m_focusedEntity->nextFocusable();
        if (nextFocused != nullptr) setFocusedEntity(nextFocused);
        else setFocusedEntity(m_layers[LayerID::NUI].root.nextFocusable());
    }

    // Find previous entity
    else if (event.key.code == sf::Keyboard::Left
        || event.key.code == sf::Keyboard::Up) {
        // Simply get previous or restart from ths end
        Entity* previousFocused = m_focusedEntity->previousFocusable();
        if (previousFocused != nullptr) setFocusedEntity(previousFocused);
        else {
            previousFocused = m_layers[LayerID::NUI].root.lastDescendant();
            if (previousFocused->focusable()) setFocusedEntity(previousFocused);
            else setFocusedEntity(previousFocused->previousFocusable());
        }
    }
}

//---------------------------//
//----- Zooming feature -----//

void Graph::handleMouseWheelPressedEvent(const sf::Event& event)
{
    const auto& window = Application::context().window;
    auto mousePos = mousePosition(event);
    m_grabbing = true;

    for (auto& layer : m_layers) {
        if (layer.manipulable) {
            auto position = window.mapPixelToCoords(mousePos, *layer.view);
            layer.grabbingPosition = position;
        }
    }
}

void Graph::handleMouseWheelReleasedEvent(const sf::Event& event)
{
    m_grabbing = false;
}

bool Graph::handleMouseMovedEvent(const sf::Event& event)
{
    returnif (m_grabbing == false) false;

    const auto& window = Application::context().window;
    auto mousePos = mousePosition(event);

    for (auto& layer : m_layers) {
        if (layer.manipulable) {
            auto position = window.mapPixelToCoords(mousePos, *layer.view);
            layer.view->move(layer.grabbingPosition - position);
        }
    }

    return true;
}

void Graph::handleMouseWheelMovedEvent(const sf::Event& event)
{
    int delta = event.mouseWheel.delta;
    auto mousePos = mousePosition(event);
    const auto& window = Application::context().window;

    // TODO Factor to be in config
    float zoomFactor = (delta < 0)? 1.05f : 0.95f;
    zoomFactor *= std::abs(delta);

    // Zoom on all zoomable layers
    for (auto& layer : m_layers) {
        if (layer.manipulable) {
            // Apply new zoom
            auto initPos = window.mapPixelToCoords(mousePos, *layer.view);
            layer.view->zoom(zoomFactor);

            // Move to adjust the point below the mouse
            auto finalPos = window.mapPixelToCoords(mousePos, *layer.view);
            layer.view->move(initPos - finalPos);
        }
    }
}

//----------------------------//
//----- Grabbing objects -----//

void Graph::grabbableHandleMouseEvent(const sf::Event& event)
{
    sf::Vector2i mousePos(mousePosition(event));
    sf::Vector2f viewPos;
    sf::Vector2f relPos;
    Entity* entity;

    const auto& window = Application::context().window;
    sf::Vector2f nuiPos = window.mapPixelToCoords(mousePos, *m_layers[LayerID::NUI].view);

    switch (event.type) {
    case sf::Event::MouseMoved:
        m_grabbable->setPosition(nuiPos);
        break;
    case sf::Event::MouseButtonReleased:
        entity = entityFromPosition(mousePos, viewPos);
        if (entity != nullptr) relPos = entity->getInverseTransform().transformPoint(viewPos);
        m_grabbable->spawner().grabbableReleased(entity, relPos, nuiPos);
        m_grabbable = nullptr;
        break;
    default:
        break;
    }
}

void Graph::setGrabbable(std::unique_ptr<Grabbable> grabbable)
{
    returnif (grabbable.get() == nullptr);

    m_grabbable = std::move(grabbable);

    // TODO Remove this duplicated code from handleMouseEvent() && grabbableHandleMouseEvent() functions
    const auto& window = Application::context().window;
    auto mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f nuiPos = window.mapPixelToCoords(mousePos, *m_layers[LayerID::NUI].view);
    m_grabbable->setPosition(nuiPos);
}

//---------------------------//
//----- Mouse detection -----//

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

Entity* Graph::handleMouseEvent(const sf::Event& event)
{
    // Let grabbable manage if any
    if (m_grabbable.get() != nullptr) {
        grabbableHandleMouseEvent(event);
        return nullptr;
    }

    sf::Vector2i mousePos(mousePosition(event));
    sf::Vector2f viewPos;

    // Getting entity touched if any
    Entity* entity = entityFromPosition(mousePos, viewPos);
    setHoveredEntity(entity);
    returnif (entity == nullptr) nullptr;

    // Getting relative coordinates
    const auto& window = Application::context().window;
    sf::Vector2f nuiPos = window.mapPixelToCoords(mousePos, *m_layers[LayerID::NUI].view);
    sf::Vector2f relPos = entity->getInverseTransform().transformPoint(viewPos);

    // Calling child callback
    switch (event.type) {
    case sf::Event::MouseMoved:
        entity->handleMouseMoved(relPos, nuiPos);
        break;
    case sf::Event::MouseButtonPressed:
        entity->handleMouseButtonPressed(event.mouseButton.button, relPos, nuiPos);
        break;
    case sf::Event::MouseButtonReleased:
        entity->handleMouseButtonReleased(event.mouseButton.button, relPos, nuiPos);
        break;
    case sf::Event::MouseWheelMoved:
        entity->handleMouseWheelMoved(event.mouseWheel.delta, relPos, nuiPos);
        break;
    default:
        break;
    }

    return entity;
}

void Graph::setHoveredEntity(Entity* hoveredEntity)
{
    returnif (m_hoveredEntity == hoveredEntity);

    // Emit MouseLeft if mouse was over an other child
    if (m_hoveredEntity != nullptr)
        m_hoveredEntity->handleMouseLeft();

    m_hoveredEntity = hoveredEntity;
}

Entity* Graph::entityFromPosition(const sf::Vector2i& mousePos, sf::Vector2f& viewPos)
{
    const auto& window = Application::context().window;

    rfor (layer, m_layers) {
        // Extrapolating position and getting entity if any
        viewPos = window.mapPixelToCoords(mousePos, *layer->view);
        Entity* entity = layer->root.firstOver(viewPos);
        returnif (entity != nullptr) entity;
    }

    return nullptr;
}

//---------------------------//
//---- Entity callbacks -----//

void Graph::detachedEntity(Entity* entity)
{
    // Cancel focus if it was
    if (m_focusedEntity == entity)
        setFocusedEntity(nullptr);
}

void Graph::attachedEntity(Entity* entity)
{
    // Attribute focus if there is not yet
    if (m_focusedEntity == nullptr && entity->focusable())
        setFocusedEntity(entity);
}
