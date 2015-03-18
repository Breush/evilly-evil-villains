#pragma once

#include "nui/hstacker.hpp"
#include "nui/imagebutton.hpp"
#include "scene/entity.hpp"
#include "tools/param.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <array>

namespace dungeon
{
    // Forward declaration
    class Sidebar;

    class Panel : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:
        Panel(dungeon::Sidebar& sidebar);
        virtual ~Panel() = default;

        // Reduced mode
        void setReduced(bool reduced);
        void switchReduced();

    protected:
        // Virtual
        void update() override;

        // Mouse events
        void handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        void handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        void handleMouseLeft() override;

        // Params
        PARAMG(bool, m_reduced, reduced)

    private:
        sf::RectangleShape m_background;
        sf::RectangleShape m_switchReducedButton;

        // Tabs
        nui::HStacker m_tabsStacker;
        std::array<nui::ImageButton, 4> m_tabs;

        // Sidebar
        Sidebar& m_sidebar;
    };
}
