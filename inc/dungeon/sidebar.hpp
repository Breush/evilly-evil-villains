#pragma once

#include "nui/vstacker.hpp"
#include "nui/grabbutton.hpp"
#include "scene/entity.hpp"
#include "dungeon/event.hpp"
#include "dungeon/panel.hpp"
#include "tools/param.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace dungeon
{
    class Sidebar final : public scene::Entity, public EventReceiver
    {
        using baseClass = scene::Entity;

    public:
        Sidebar();
        ~Sidebar();

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

        void receive(const Event& event) final;

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! Whether the panel is currently reduced or not.
        PARAMG(bool, m_reduced, reduced)

        //! @}

    private:

        // Decorum
        sf::RectangleShape m_background;    //!< The background.

        // Stacker
        nui::VStacker m_globalStacker;  //!< Contains all elements.

        // Tabs
        dungeon::Panel m_tabs;              //!< The tabs.
        nui::VStacker m_tabContentStacker;  //!< The stacker containing the current tab content.
        std::vector<std::unique_ptr<nui::GrabButton>> m_tabContent; //!< The dynamic content for the tabs.
    };
}
