#pragma once

#include "nui/hstacker.hpp"
#include "scene/entity.hpp"
#include "tools/param.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace dungeon
{
    class Sidebar : public scene::Entity
    {
        typedef scene::Entity baseClass;

    public:
        Sidebar();
        virtual ~Sidebar() {}

        // Reduced mode
        void setReduced(bool reduced);
        void switchReduced();
        void immediateReduce();

    protected:
        // Virtual
        void update() override;

        // Mouse events
        void handleMouseButtonPressed(const sf::Mouse::Button& button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        void handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        void handleMouseLeft() override;

        // Params
        PARAMG(bool, m_reduced, reduced)

    private:
        sf::RectangleShape m_background;
        sf::RectangleShape m_switchReducedButton;

        // Tabs
        nui::HStacker m_tabsStacker;
    };
}
