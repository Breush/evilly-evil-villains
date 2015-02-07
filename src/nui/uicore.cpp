#include "core/application.hpp"

#include "nui/uicore.hpp"
#include "nui/object.hpp"

#include "resources/holder.hpp"
#include "resources/soundplayer.hpp"

#include "tools/event.hpp"
#include "tools/debug.hpp"
#include "tools/tools.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Sprite.hpp>

using namespace nui;

uiCore::uiCore()
    : m_hoveredChild(nullptr)
    , m_focusedChild(nullptr)
    , m_forgottenFocusedChild(nullptr)
    , m_focusAnimation(0)
{
    // Hovering system
    m_mouseDetector.init();

    // Focusing system
    m_focusSprite.setTexture(Application::context().textures.get(Textures::NUI_FOCUS));
    m_focusSprite.setColor( {255, 255, 255, 100});
}

uiCore::~uiCore()
{
    m_children.clear();
    m_focusedChild = nullptr;
}

//-------------------------------//
//----- Children management -----//

void uiCore::add(Object* child)
{
    m_children.push_back(child);
    child->m_core = this;
    child->init();

    // Add to detection
    m_mouseDetector.add(child);

    // First child gets focus
    if (focusedChild() == nullptr)
        setFocusedChild(child);

}

//------------------//
//----- Events -----//

void uiCore::handleEvent(const sf::Event& event)
{
    // Window events are not considered (maybe later)
    returnif (isWindow(event));

    // Delegate for mouse
    if (isMouse(event)) {
        interaction::Detectable* child = m_mouseDetector.handleMouseEvent(event);
        // Focus on click
        if (child != nullptr && event.type == sf::Event::MouseButtonPressed)
            setFocusedChild(child->asObject());
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

    // Detection
    // TODO Colors are not visible
    debug_nui_2(m_mouseDetector.draw());
}

void uiCore::update(const sf::Time& dt)
{
    // Checking for children updates
    for (auto& child : m_children) {
        // Child is focused - update shader
        if (child == focusedChild() && child->status()) {
            sf::FloatRect r(child->getTransform().transformRect(child->focusRect()));
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
    m_mouseDetector.update(dt);
}

void uiCore::refresh()
{
    for (auto& child : m_children)
        child->refresh();
}
