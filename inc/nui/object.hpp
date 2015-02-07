#pragma once

#include "resources/identifiers.hpp"
#include "interaction/detectable.hpp"
#include "tools/param.hpp"
#include "tools/tools.hpp"

#include <SFML/System.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Drawable.hpp>

namespace nui
{
    class uiCore;

    class Object : public sf::Drawable, public interaction::Detectable
    {
        typedef interaction::Detectable baseClass;
        friend class uiCore;

    public:
        Object();
        virtual ~Object() {}

        // Init or update
        virtual void init() = 0;
        virtual void update() = 0;

        // Events
        virtual bool handleKeyboardEvent(const sf::Event&) { return false; }
        virtual bool handleJoystickEvent(const sf::Event&) { return false; }

        // Virtual
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void update(const sf::Time& dt) override { baseClass::update(dt); }

    protected:
        // Sub-parts of drawing
        struct Part {
            sf::Drawable* drawable;
            Shaders::ID shader;
            sf::IntRect* clippingRect;
        };

        void clearParts();
        void resetPartsShader();
        void removePart(sf::Drawable* drawable);
        void setPartShader(sf::Drawable* drawable, Shaders::ID shader);
        void setPartClippingRect(sf::Drawable* drawable, sf::IntRect* clippingRect);
        void addPart(sf::Drawable* drawable, Shaders::ID shader = Shaders::NONE);

        // Updates
        virtual void changedSize() override;
        virtual void changedFocusRect();
        virtual void changedVisible();

        // Focus interactions
        virtual bool ownsFocus() { return false; }

        // Params
        PARAMG(uiCore*, m_core, core)
        PARAMGS(bool, m_centered, centered, setCentered)
        PARAMGS(bool, m_focusable, focusable, setFocusable)
        PARAMGSU(bool, m_visible, visible, setVisible, changedVisible)
        PARAMGSU(sf::FloatRect, m_focusRect, focusRect, setFocusRect, changedFocusRect)

    private:
        std::vector<Part> m_parts;
    };
}
