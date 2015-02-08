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

        void init();
        void update(const sf::Time& dt);
        void draw();

        // Children management
        void add(Detectable* child);
        void draw(const Detectable& child, sf::RenderStates states);

        // Debug drawing to see hitboxes
        void debugDraw() const;

        // Events - detect mouse and pick child
        Detectable* handleMouseEvent(const sf::Event& event);

    protected:
        // Stocking children info
        struct ChildInfo {
            sf::Color color;
            Detectable* child;
        };

        // Hovering
        void setHoveredChild(Detectable* inHoveredChild);

        // Low-level
        void increaseColor();
        Detectable* childFromPosition(const sf::Vector2u& pos) const;

    private:
        // Mouse picking
        bool m_needRedraw;
        sf::Color m_color;
        sf::Image m_image;
        sf::RenderTexture m_texture;
        std::list<ChildInfo> m_childrenList;

        // Hovering
        Detectable* m_hoveredChild;

    };
}
