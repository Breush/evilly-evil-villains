#pragma once

#include "interaction/detectable.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <list>

namespace interaction
{
    class MouseDetector
    {
    public:
        MouseDetector();
        virtual ~MouseDetector() {}

        // Children management
        void add(Detectable* child);

        // Debug drawing to see hitboxes
        void draw() const;

        // Events - detect mouse and pick child
        Detectable* handleMouseEvent(const sf::Event& event);

    protected:
        // Hovering
        void setHoveredChild(Detectable* inHoveredChild);

        // Low-level
        Detectable* childFromPosition(const sf::Vector2f& mousePos) const;

    private:
        // Mouse picking
        std::list<Detectable*> m_children;

        // Hovering
        Detectable* m_hoveredChild;

    };
}
