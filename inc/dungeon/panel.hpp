#pragma once

#include "nui/object.hpp"
#include "nui/hstacker.hpp"
#include "nui/button.hpp"

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

    protected:
        // Mouse events
        void handleMousePressed(const sf::Event& event, const sf::Vector2f& absPos, const sf::Vector2f& relPos);
        void handleMouseMoved(const sf::Event& event, const sf::Vector2f& absPos, const sf::Vector2f& relPos);
        void handleMouseLeft();

        // Changes
        void changedSize() override;

    private:
        sf::RectangleShape m_bg; // TODO To be a sf::Sprite for looks

        // Tabs
        nui::HStacker m_tabsStacker;
        // TODO Make a tab nui Object with is clickable text + image
        nui::Button m_monstersTab;
        nui::Button m_trapsTab;
        nui::Button m_facilitiesTab;
        nui::Button m_tresuresTab;
    };
}
