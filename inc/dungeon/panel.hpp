#pragma once

#include "nui/object.hpp"
#include "nui/hstacker.hpp"
#include "nui/imagebutton.hpp"
#include "tools/param.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace dungeon
{
    class Panel : public nui::Object
    {
        typedef nui::Object baseClass;

    public:
        Panel();
        virtual ~Panel() {}

        // Virtual
        void init() override;
        void update() override;

        // Reduced mode
        void switchReduced();

    protected:
        // Mouse events
        void handleMouseButtonPressed(const sf::Mouse::Button& mouseButton, const sf::Vector2f& mousePos) override;
        void handleMouseMoved(const sf::Vector2f& mousePos) override;
        void handleMouseLeft() override;

        // Changes
        void changedStatus() override;
        void changedSize() override;

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
