#pragma once

#include "nui/hstacker.hpp"
#include "nui/imagebutton.hpp"
#include "scene/entity.hpp"
#include "tools/param.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace dungeon
{
    class Panel : public scene::Entity
    {
        typedef scene::Entity baseClass;

    public:
        Panel();
        virtual ~Panel() {}

        // Reduced mode
        void switchReduced();

    protected:
        // Virtual
        void update() override;

        // Mouse events
        void handleMouseButtonPressed(const sf::Mouse::Button& mouseButton, const sf::Vector2f& mousePos) override;
        void handleMouseMoved(const sf::Vector2f& mousePos) override;
        void handleMouseLeft() override;

        // Params
        PARAMGS(bool, m_reduced, reduced, setReduced)
        PARAMGS(sf::Vector2f, m_reducedVector, reducedVector, setReducedVector)

    private:
        sf::RectangleShape m_background;
        sf::RectangleShape m_switchReducedButton;

        // Tabs
        nui::HStacker m_tabsStacker;
        nui::ImageButton m_monstersTab;
        nui::ImageButton m_trapsTab;
        nui::ImageButton m_facilitiesTab;
        nui::ImageButton m_treasuresTab;
    };
}
