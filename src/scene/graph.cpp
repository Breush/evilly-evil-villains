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
    for (auto& layerRootEntity : m_layers) {
        layerRootEntity.setDetectable(false);
        layerRootEntity.setFocusable(false);
        layerRootEntity.setVisible(false);
        layerRootEntity.setGraph(this);
    }
}

//-------------------//
//----- Routine -----//

void Graph::update(const sf::Time& dt)
{
    // Update recursively all the entities
    for (auto& layerRootEntity : m_layers)
        layerRootEntity.update(dt);

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
    for (auto& layerRootEntity : m_layers)
        target.draw(layerRootEntity, states);

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
        else setFocusedEntity(m_layers[Layers::NUI].nextFocusable());
    }

    // Find previous entity
    else if (event.key.code == sf::Keyboard::Left
        || event.key.code == sf::Keyboard::Up) {
        // Simply get previous or restart from ths end
        Entity* previousFocused = m_focusedEntity->previousFocusable();
        if (previousFocused != nullptr) setFocusedEntity(previousFocused);
        else {
            previousFocused = m_layers[Layers::NUI].lastDescendant();
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

    // TODO How to know the view on the hovered child?
    sf::RectangleShape rectangleShape;
    rectangleShape.setFillColor({255, 0, 0, 150});
    rectangleShape.setSize(m_hoveredEntity->size());
    rectangleShape.setPosition(m_hoveredEntity->getPosition());
    rectangleShape.setRotation(m_hoveredEntity->getRotation());
    rectangleShape.setScale(m_hoveredEntity->getScale());
    rectangleShape.setOrigin(m_hoveredEntity->getOrigin());

    target.draw(rectangleShape);
}

Entity* Graph::handleMouseEvent(const sf::Event& event)
{
    // TODO /!\ Use view concerned by layer

    // Extrapolating position
    auto& window = Application::context().window;
    const auto& defaultView = Application::context().views.get(Views::DEFAULT);
    sf::Vector2f absPos(window.mapPixelToCoords(mousePosition(event), defaultView));
    clamp(absPos, Application::context().resolution);

    // Getting entity touched if any
    Entity* entity = entityFromPosition(absPos);
    setHoveredEntity(entity);
    returnif (entity == nullptr) nullptr;

    // Getting relative coordinates
    sf::Vector2f relPos = entity->getInverseTransform().transformPoint(absPos);

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

Entity* Graph::entityFromPosition(const sf::Vector2f& mousePos)
{
    // TODO Only in a layer, passed by argument
    rfor (layerRootEntity, m_layers) {
        Entity* entity = (*layerRootEntity).firstOver(mousePos);
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
