#pragma once

#include "states/state.hpp"
#include "resources/identifiers.hpp"
#include "interaction/mousedetector.hpp"
#include "tools/param.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

#include <list>

namespace nui
{
    class Object;

    class uiCore : public sf::Drawable
    {
    public:
        uiCore();
        ~uiCore();

        // Main functions called by states
        void refresh();
        void update(const sf::Time& dt);
        void handleEvent(const sf::Event& event);
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        // Children management
        void add(Object* child);

        // Focusing system
        void setFocusedChild(Object* inFocusedChild);
        void forgetFocusedChild();
        void rememberFocusedChild();

    protected:

        // Focusing system
        void updateFocusSprite();
        void manageFocusedChild(const sf::Event& event);

        // Params
        PARAMG(Object*, m_hoveredChild, hoveredChild)
        PARAMG(Object*, m_focusedChild, focusedChild)
        PARAMGS(sf::IntRect, m_focusRect, focusRect, setFocusRect)

    private:
        // Children management
        std::list<Object*> m_children;

        // Focusing system
        Object* m_forgottenFocusedChild;
        sf::Sprite m_focusSprite;
        int m_focusAnimation;

        // Hovering system
        interaction::MouseDetector m_mouseDetector;
    };
}
