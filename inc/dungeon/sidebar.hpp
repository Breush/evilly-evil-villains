#pragma once

#include "nui/vstacker.hpp"
#include "nui/grabbutton.hpp"
#include "scene/entity.hpp"
#include "tools/param.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace dungeon
{
    class Sidebar : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:
        Sidebar();
        virtual ~Sidebar() = default;

        // Reduced mode
        void setReduced(bool reduced);
        void switchReduced();
        void immediateReduce();

        // Mode
        enum class Mode {
            MONSTERS,
            TRAPS,
            FACILITIES,
            TREASURES,
        };

        void setMode(Mode mode);

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
        std::vector<std::unique_ptr<nui::GrabButton>> m_tabs;
        nui::VStacker m_tabsStacker;
    };
}
