#pragma once

#include "tools/param.hpp"
#include "sfe/localtransformable.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Window/Event.hpp>

// Forward declaration
namespace nui
{
    class Object;
}

namespace interaction
{
    // Forward declaration
    class MouseDetector;

    class Detectable : public sfe::LocalTransformable
    {
        friend class MouseDetector;
        typedef sfe::LocalTransformable baseClass;

    public:
        Detectable();
        virtual ~Detectable();

        // Polymorphism
        virtual nui::Object* asObject() { return nullptr; }

        // Calls mouseDetector drawing function
        void detectableDraw(sf::RenderStates states) const;

    protected:
        // Changes
        virtual void changedSize() {}

        // Events
        virtual void handleMouseMoved(const sf::Vector2f& mousePos) {}
        virtual void handleMouseButtonPressed(const sf::Mouse::Button& mouseButton, const sf::Vector2f& mousePos) {}
        virtual void handleMouseButtonReleased(const sf::Mouse::Button& mouseButton, const sf::Vector2f& mousePos) {}
        virtual void handleMouseWheelMoved(const int& delta, const sf::Vector2f& mousePos) {}
        virtual void handleMouseLeft() {}

        // Params
        PARAMGS(bool, m_detectable, detectable, setDetectable)
        PARAMGS(MouseDetector*, m_mouseDetector, mouseDetector, setMouseDetector)
        PARAMGSU(sf::Vector2f, m_size, size, setSize, changedSize)
    };
}