#pragma once

#include "resources/identifiers.hpp"
#include "tools/param.hpp"
#include "tools/tools.hpp"

#include <SFML/System.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

namespace nui
{
    class uiCore;

    class Object : public sf::Drawable, public sf::Transformable
    {
        friend class uiCore;

    public:
        Object();
        virtual ~Object() {}

        // Init or update
        virtual void init() = 0;
        virtual void update() = 0;

        // Events
        virtual void handleMouseEvent(const sf::Event&, const sf::Vector2f& absPos, const sf::Vector2f& relPos) {}
        virtual bool handleKeyboardEvent(const sf::Event&) { return false; }
        virtual bool handleJoystickEvent(const sf::Event&) { return false; }

        // Virtual
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        virtual void update(sf::Time dt);

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
        virtual void changedSize();
        virtual void changedParent();
        virtual void changedFocusRect();
        virtual void changedVisible();
        virtual void changedChild(Object* child);

        // Focus interactions
        virtual bool ownsFocus() { return false; }

        // Parent-related
        sf::Vector2f getGlobalPosition() const;
        sf::Transform getGlobalTransform() const;
        void applyGlobalTransform(sf::Transform& tansform) const;

        // Params
        PARAMG(uiCore*, m_core, core)
        PARAMGS(bool, m_status, status, setStatus)
        PARAMGS(bool, m_centered, centered, setCentered)
        PARAMGS(bool, m_focusable, focusable, setFocusable)
        PARAMGS(bool, m_detectable, detectable, setDetectable)
        PARAMGSU(bool, m_visible, visible, setVisible, changedVisible)
        PARAMGSU(sf::FloatRect, m_focusRect, focusRect, setFocusRect, changedFocusRect)
        PARAMGSU(Object*, m_parent, parent, setParent, changedParent)
        PARAMGSU(sf::Vector2f, m_size, size, setSize, changedSize)

    private:
        std::vector<Part> m_parts;
    };
}
