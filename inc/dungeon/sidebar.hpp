#pragma once

#include "nui/vstacker.hpp"
#include "nui/grabbutton.hpp"
#include "scene/entity.hpp"
#include "dungeon/event.hpp"
#include "tools/param.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace dungeon
{
    // FIXME So much duplicated code with dungeon::Panel

    class Sidebar final : public scene::Entity, public EventReceiver
    {
        using baseClass = scene::Entity;

    public:
        Sidebar();
        ~Sidebar();

        // Reduced mode
        void setReduced(bool reduced);
        void switchReduced();
        void immediateReduce();

        // Mode
        enum class Mode {
            MONSTERS,
            TRAPS,
            FACILITIES,
        };

        void setMode(Mode mode);

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

        //--------------------------//
        //! @name Public properties
        //! @{

        //! Whether the panel is currently reduced or not.
        PARAMG(bool, m_reduced, reduced)

        //! @}

    private:
        sf::RectangleShape m_background;
        sf::RectangleShape m_switchReducedButton;

        // Tabs
        std::vector<std::unique_ptr<nui::GrabButton>> m_tabs;
        nui::VStacker m_tabsStacker;
    };
}
