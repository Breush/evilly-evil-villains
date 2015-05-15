#pragma once

#include "nui/hstacker.hpp"
#include "nui/imagebutton.hpp"
#include "scene/entity.hpp"
#include "dungeon/event.hpp"
#include "dungeon/data.hpp"
#include "tools/param.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <array>

namespace dungeon
{
    // Forward declarations

    class Sidebar;

    //! A simple panel to select objects to grab to the dungeon.

    class Panel final : public scene::Entity, public EventReceiver
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        Panel(dungeon::Sidebar& sidebar);

        //! Default destructor.
        virtual ~Panel() = default;

        // Reduced mode
        void setReduced(bool reduced);
        void switchReduced();

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        void handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        void receive(const Event& event) final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        //! Whether the panel is currently reduced or not.
        PARAMG(bool, m_reduced, reduced)

        //! @}

    private:

        // Sidebar
        Sidebar& m_sidebar;

        // Tabs
        nui::HStacker m_tabsStacker;
        std::array<nui::ImageButton, 3> m_tabs;

        // Decorum
        sf::RectangleShape m_background;
        sf::RectangleShape m_switchReducedButton;
    };
}
