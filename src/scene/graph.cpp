#include "scene/graph.hpp"

#include "core/application.hpp"
#include "tools/math.hpp"
#include "tools/event.hpp"
#include "tools/tools.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

using namespace scene;

Graph::Graph()
    : m_hoveredEntity(nullptr)
    , m_focusedEntity(nullptr)
    , m_focusAnimation(0.f)
{
    // Focusing
    m_focusShader = &Application::context().shaders.get(Shaders::NUI_FOCUS);
    m_focusShape.setTexture(&Application::context().textures.get(Textures::NUI_FOCUS));
    m_focusShape.setFillColor({255, 255, 255, 100});

    // Default parameters
    for (auto& layer : m_layers) {
        layer.root.setDetectable(false);
        layer.root.setFocusable(false);
        layer.root.setVisible(false);
        layer.root.setGraph(this);
    }

    // Initialisation of layers views
    m_layers[Layers::DUNGEON_DESIGN].view = &Application::context().views.get(Views::DUNGEON_DESIGN);
    m_layers[Layers::NUI].view = &Application::context().views.get(Views::NUI);
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
        auto entity = handleMouseEvent(event);

        // Set focus on click
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

    // Hovered child on debug
    debug_nui_1(drawMouseDetector(target, states));
}

//--------------------//
//----- Focusing -----//

void Graph::updateFocusSprite()
{
    const sf::FloatRect& focusRect = m_focusedEntity->focusRect();

    sf::Vector2f focusPosition(focusRect.left, focusRect.top);
    sf::Vector2f focusSize(focusRect.width, focusRect.height);

    m_focusShape.setPosition(focusPosition);
    m_focusShape.setSize(focusSize);

    sf::Vector2f globalFocusPosition = m_focusedEntity->getPosition() - m_focusedEntity->getOrigin() + focusPosition;

    Application::context().shaders.setParameter(Shaders::NUI_FOCUS, "position", globalFocusPosition);
    Application::context().shaders.setParameter(Shaders::NUI_FOCUS, "textureSize", focusSize);
}

void Graph::setFocusedEntity(Entity* focusedEntity)
{
    returnif (focusedEntity == nullptr);
    returnif (!focusedEntity->focusable());
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

    // Just manage pressed keys - TODO Joystick
    returnif (event.type != sf::Event::KeyPressed);

    // Find next entity
    if (event.key.code == sf::Keyboard::Right
        || event.key.code == sf::Keyboard::Down
        || event.key.code == sf::Keyboard::Tab) {
        // Simply get next or restart from the beginning
        Entity* nextFocused = m_focusedEntity->nextFocusable();
        if (nextFocused != nullptr) setFocusedEntity(nextFocused);
        else setFocusedEntity(m_layers[Layers::NUI].root.nextFocusable());
    }

    // Find previous entity
    else if (event.key.code == sf::Keyboard::Left
        || event.key.code == sf::Keyboard::Up) {
        // Simply get previous or restart from ths end
        Entity* previousFocused = m_focusedEntity->previousFocusable();
        if (previousFocused != nullptr) setFocusedEntity(previousFocused);
        else {
            previousFocused = m_layers[Layers::NUI].root.lastDescendant();
            if (previousFocused->focusable()) setFocusedEntity(previousFocused);
            else setFocusedEntity(previousFocused->previousFocusable());
        }
    }
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
    // Getting entity touched if any
    sf::Vector2f viewPos;
    Entity* entity = entityFromPosition(mousePosition(event), viewPos);
    setHoveredEntity(entity);
    returnif (entity == nullptr) nullptr;

    // Getting relative coordinates
    sf::Vector2f relPos = entity->getInverseTransform().transformPoint(viewPos);

    // Calling child callback
    if (event.type == sf::Event::MouseMoved)
        entity->handleMouseMoved(relPos);
    else if (event.type == sf::Event::MouseButtonPressed)
        entity->handleMouseButtonPressed(event.mouseButton.button, relPos);
    else if (event.type == sf::Event::MouseButtonReleased)
        entity->handleMouseButtonReleased(event.mouseButton.button, relPos);
    else if (event.type == sf::Event::MouseWheelMoved)
        entity->handleMouseWheelMoved(event.mouseWheel.delta, relPos);

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
