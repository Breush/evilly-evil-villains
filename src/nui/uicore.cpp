#include "core/application.hpp"

#include "nui/uicore.hpp"
#include "nui/object.hpp"

#include "resources/holder.hpp"
#include "resources/soundplayer.hpp"

#include "tools/event.hpp"
#include "tools/debug.hpp"
#include "tools/tools.hpp"
#include "tools/math.hpp" // clamp

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Sprite.hpp>

using namespace nui;

uiCore::uiCore()
    : m_viewSize(Application::context().resolution)
    , m_hoveredChild(nullptr)
    , m_focusedChild(nullptr)
    , m_focusAnimation(0)
{
    // Hovering system
    m_detectImage.create(viewSize().x, viewSize().y);
    m_detectTarget.create(viewSize().x, viewSize().y);
    m_detectTarget.setSmooth(false);

    // Focusing system
    m_focusSprite.setTexture(Application::context().textures.get(Textures::NUI_FOCUS));
    m_focusSprite.setColor( {255, 255, 255, 100});
}

uiCore::~uiCore()
{
    m_children.clear();
    m_detectMap.clear();
    m_focusedChild = nullptr;
}

//-------------------------------//
//----- Children management -----//

void uiCore::add(Object* child)
{
    child->m_core = this;
    child->init();

    // First child gets focus
    if (focusedChild() == nullptr)
        setFocusedChild(child);

    m_children.push_back(child);
    if ( child->detectable() )
        addToDetectMap(child);
}

void uiCore::addToDetectMap(Object* object)
{
    // Increase current color (limited to red for now)
    massert(m_detectColor.r != 255, "Object number exceeds core limit");
    ++m_detectColor.r;

    m_detectMap[m_detectColor.r] = object;
}

//-------------------------//
//----- Detect buffer -----//

void uiCore::drawDetectImage()
{
    m_detectTarget.clear(sf::Color::Black);

    // /!\ UI Debug mode
    debug(2, uint8 blueColor = 0);
    debug(2, uint8 greenColor = 255);

    for (auto& it : m_detectMap) {
        sf::RectangleShape rectangleShape(it.second->size());
        rectangleShape.setPosition(it.second->getPosition());
        rectangleShape.setOrigin(it.second->getOrigin());
        rectangleShape.setFillColor(sf::Color(it.first, 0, 0));
        debug(2, rectangleShape.setFillColor( {it.first, greenColor, blueColor}));
        debug(2, blueColor += 25);
        debug(2, greenColor -= 25);
        m_detectTarget.draw(rectangleShape);
    }

    m_detectTarget.display();
    m_detectImage = m_detectTarget.getTexture().copyToImage();
}

uint8 uiCore::getDetectValue(const sf::Vector2u& pos) const
{
    return getDetectValue(pos.x, pos.y);
}

uint8 uiCore::getDetectValue(const uint& x, const uint& y) const
{
    return m_detectImage.getPixel(x, y).r;
}

//------------------//
//----- Events -----//

void uiCore::handleEvent(const sf::Event& event)
{
    // Window events are not considered (maybe later)
    returnif (isWindow(event));

    // Mouse moving - setting hovered child
    if (event.type == sf::Event::MouseMoved) {
        auto& window = Application::context().window;
        sf::Vector2f relPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
        clamp(relPos, Application::context().resolution);

        // Does mouse select any child?
        auto object = m_detectMap.find(getDetectValue(uint(relPos.x), uint(relPos.y)));
        if (object == m_detectMap.end()) {
            setHoveredChild(nullptr);
            return;
        }

        setHoveredChild(object->second);
        object->second->handleMouseEvent(event, relPos);
        return;
    }

    // Mouse : click or wheel action
    if (isMouse(event)) {
        auto& window = Application::context().window;
        sf::Vector2f relPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        clamp(relPos, Application::context().resolution);

        auto object = m_detectMap.find(getDetectValue(uint(relPos.x), uint(relPos.y)));
        returnif (object == m_detectMap.end());

        setFocusedChild(object->second);
        object->second->handleMouseEvent(event, relPos);
        return;
    }

    // Keyboard or Joystick
    auto object = focusedChild();
    returnif (object == nullptr);

    bool grabbedFocus = (isKeyboard(event))? object->handleKeyboardEvent(event)
                        : object->handleJoystickEvent(event);

    if (!grabbedFocus)
        manageFocusedChild(event);
}

//--------------------//
//----- Focusing -----//

void uiCore::setFocusedChild(Object* inFocusedChild)
{
    returnif (!inFocusedChild->focusable());
    returnif (m_focusedChild == inFocusedChild);

    m_focusedChild = inFocusedChild;
    if (m_focusedChild != nullptr)
        m_focusedChild->setStatus(true);
}

void uiCore::manageFocusedChild(const sf::Event& event)
{
    massert(focusedChild() != nullptr, "Internal logical error for focus");

    // Be sure to find one possible child
    bool focusPossible = false;
    for (auto& child : m_children)
        focusPossible |= (child->focusable());
    returnif (!focusPossible);

    // Just manage pressed keys
    returnif (event.type != sf::Event::KeyPressed);
    // TODO Manage Joystick

    // Find next child
    if (event.key.code == sf::Keyboard::Right
            || event.key.code == sf::Keyboard::Down) {
        bool nextChild = false;
        for (auto& child : m_children) {
            if (nextChild) {
                if (child->focusable()) {
                    setFocusedChild(child);
                    return;
                }
           } else {
                nextChild = (child == focusedChild());
           }
        }

        // Whole loop, next child is the first possible
        for (auto& child : m_children) {
            if (child->focusable())
            {
                setFocusedChild(child);
                return;
            }
        }
    }

    // Find previous child
    if (event.key.code == sf::Keyboard::Left
            || event.key.code == sf::Keyboard::Up) {
        bool prevChild = false;
        rfor (child, m_children) {
            if (prevChild) {
                if ((*child)->focusable()) {
                    setFocusedChild(*child);
                    return;
                }
            } else {
                prevChild = (*child == focusedChild());
            }
        }

        // Whole loop, next child is the first possible
        rfor (child, m_children) {
            if ((*child)->focusable())
            {
                setFocusedChild(*child);
                return;
            }
        }
    }

}

//--------------------//
//----- Hovering -----//

void uiCore::setHoveredChild(Object* inHoveredChild)
{
    returnif (m_hoveredChild == inHoveredChild);

    if (m_hoveredChild == nullptr) {
        m_hoveredChild = inHoveredChild;
        return;
    }

    // Emit MouseLeft if mouse was over an other child
    sf::Event event;
    event.type = sf::Event::MouseLeft;
    m_hoveredChild->handleMouseEvent(event, sf::Vector2f());
    m_hoveredChild = inHoveredChild;
}

//-------------------//
//----- Drawing -----//

void uiCore::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Drawing children
    for (auto& child : m_children) {
        // Mark focused child
        if (child == focusedChild()) {
            sf::RenderStates focusStates(states);
            focusStates.shader = &Application::context().shaders.get(Shaders::NUI_FOCUS);
            target.draw(m_focusSprite, focusStates);
        }
        target.draw(*child, states);
    }

    // /!\ UI Debug mode
    debug(2, sf::Sprite sprite(m_detectTarget.getTexture()));
    debug(2, sprite.setColor({255, 255, 255, 120}));
    debug(2, target.draw(sprite));
}

void uiCore::update(sf::Time dt)
{
    bool redrawDetectImage = false;

    for (auto& child : m_children) {
        redrawDetectImage |= child->status();

        if (child == focusedChild() && child->status()) {
            setFocusRect(sf::IntRect(child->focusRect()));
            m_focusSprite.setPosition(child->getPosition());
            m_focusSprite.setOrigin(child->getOrigin());
            m_focusSprite.move(focusRect().left, focusRect().top);
            sf::Vector2f textureSize(focusRect().width, focusRect().height);
            sf::Vector2i position = Application::context().window.mapCoordsToPixel(m_focusSprite.getPosition() - m_focusSprite.getOrigin());
            Application::context().shaders.setParameter(Shaders::NUI_FOCUS, "textureSize", textureSize);
            Application::context().shaders.setParameter(Shaders::NUI_FOCUS, "position", sf::Vector2f(position.x, position.y));
        }

        child->update(dt);
    }

    // Focusing system
    if (focusedChild() != nullptr) {
        ++m_focusAnimation;
        m_focusSprite.setTextureRect( {-m_focusAnimation, -m_focusAnimation, focusRect().width, focusRect().height});
    }

    // Hovering system
    if (redrawDetectImage)
        drawDetectImage();
}

void uiCore::refresh()
{
    // Refresh focused child
    if (focusedChild())
        focusedChild()->setStatus(true);
}
