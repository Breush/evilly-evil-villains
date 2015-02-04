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
    , m_forgottenFocusedChild(nullptr)
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
    // First child gets focus
    if (focusedChild() == nullptr)
        setFocusedChild(child);

    m_children.push_back(child);
    if (child->detectable())
        addToDetectMap(child);

    child->m_core = this;
    child->init();
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
    debug_nui_2(uint8 blueColor = 0);
    debug_nui_2(uint8 greenColor = 255);

    for (auto& pair : m_detectMap) {
        if (pair.second->visible()) {
            sf::RectangleShape rectangleShape;
            rectangleShape.setFillColor(sf::Color(pair.first, 0, 0));

            // /!\ UI Debug mode - shapes are more visible
            debug_nui_2(rectangleShape.setFillColor({pair.first, greenColor, blueColor}));
            debug_nui_2(blueColor += 25);
            debug_nui_2(greenColor -= 25);

            // Draw main object
            sf::FloatRect r({0.f, 0.f, pair.second->size().x, pair.second->size().y});
            r = pair.second->getGlobalTransform().transformRect(r);
            rectangleShape.setPosition({r.left, r.top});
            rectangleShape.setSize({r.width, r.height});
            m_detectTarget.draw(rectangleShape);
        }
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
        sf::Vector2f absPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
        clamp(absPos, Application::context().resolution);

        // Does mouse select any child?
        auto object = m_detectMap.find(getDetectValue(uint(absPos.x), uint(absPos.y)));
        if (object == m_detectMap.end()) {
            setHoveredChild(nullptr);
            return;
        }

        setHoveredChild(object->second);
        sf::Vector2f relPos = object->second->getGlobalTransform().getInverse().transformPoint(absPos);
        object->second->handleMouseEvent(event, absPos, relPos);
        return;
    }

    // Mouse : click or wheel action
    if (isMouse(event)) {
        auto& window = Application::context().window;
        sf::Vector2f absPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        clamp(absPos, Application::context().resolution);

        auto object = m_detectMap.find(getDetectValue(uint(absPos.x), uint(absPos.y)));
        returnif (object == m_detectMap.end());

        setFocusedChild(object->second);
        sf::Vector2f relPos = object->second->getGlobalTransform().getInverse().transformPoint(absPos);
        object->second->handleMouseEvent(event, absPos, relPos);
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

void uiCore::forgetFocusedChild()
{
    m_forgottenFocusedChild = m_focusedChild;
    m_focusedChild = nullptr;
    refresh();
}

void uiCore::rememberFocusedChild()
{
    massert(m_forgottenFocusedChild != nullptr, "Calling remember focused without forgetting it previously.");

    m_focusedChild = m_forgottenFocusedChild;
    m_forgottenFocusedChild = nullptr;
    refresh();
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
    m_hoveredChild->handleMouseEvent(event, sf::Vector2f(), sf::Vector2f());
    m_hoveredChild = inHoveredChild;
}

//-------------------//
//----- Drawing -----//

void uiCore::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Drawing children
    for (auto& child : m_children)
        if (child->visible())
            target.draw(*child, states);

    // Focusing system
    if (focusedChild() && focusedChild()->visible()) {
        sf::RenderStates focusStates(states);
        focusStates.shader = &Application::context().shaders.get(Shaders::NUI_FOCUS);
        target.draw(m_focusSprite, focusStates);
    }

    // /!\ UI Debug mode
    debug_nui_2(sf::Sprite sprite(m_detectTarget.getTexture()));
    debug_nui_2(sprite.setColor({255, 255, 255, 150}));
    debug_nui_2(target.draw(sprite));
}

void uiCore::update(const sf::Time& dt)
{
    bool redrawDetectImage = false;

    // Checking for children updates
    for (auto& child : m_children) {
        redrawDetectImage |= child->status();

        // Child is focused - update shader
        if (child == focusedChild() && child->status()) {
            sf::FloatRect r(child->getGlobalTransform().transformRect(child->focusRect()));
            m_focusSprite.setPosition(r.left, r.top);
            setFocusRect(sf::IntRect(r));

            Application::context().shaders.setParameter(Shaders::NUI_FOCUS, "textureSize", sf::Vector2f(r.width, r.height));
            Application::context().shaders.setParameter(Shaders::NUI_FOCUS, "position", m_focusSprite.getPosition());
        }

        child->update(dt);
    }

    // Focusing system - animation
    if (focusedChild() != nullptr) {
        ++m_focusAnimation;
        m_focusSprite.setTextureRect({-m_focusAnimation, -m_focusAnimation, focusRect().width, focusRect().height});
    }

    // Hovering system
    if (redrawDetectImage)
        drawDetectImage();
}

void uiCore::refresh()
{
    for (auto& child : m_children)
        child->setStatus(true);
}
