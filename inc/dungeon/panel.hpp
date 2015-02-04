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
        void handleMouseEvent(const sf::Event& event, const sf::Vector2f& absPos, const sf::Vector2f& relPos) override;
        void update(const sf::Time& dt) override;

        // Reduced mode
        void switchReduced();

    protected:
        // Mouse events
        void handleMousePressed(const sf::Event& event, const sf::Vector2f& absPos, const sf::Vector2f& relPos);
        void handleMouseMoved(const sf::Event& event, const sf::Vector2f& absPos, const sf::Vector2f& relPos);
        void handleMouseLeft();

        // Changes
        void changedSize() override;
        void changedReduced();

        // Params
        PARAMGSU(bool, m_reduced, reduced, setReduced, changedReduced)
        PARAMGS(sf::Vector2f, m_reducedVector, reducedVector, setReducedVector)

    private:
        sf::RectangleShape m_bg; // TODO To be a sf::Sprite for looks
        sf::RectangleShape m_switchReducedButton;

        // Tabs
        nui::HStacker m_tabsStacker;
        // TODO Make a tab nui Object with is clickable text + image
        nui::ImageButton m_monstersTab;
        nui::ImageButton m_trapsTab;
        nui::ImageButton m_facilitiesTab;
        nui::ImageButton m_tresuresTab;

        // Animation
        sf::Vector2f m_reducedVectorAnimation;
        float m_reducedSpeedBySecond;
    };
}
