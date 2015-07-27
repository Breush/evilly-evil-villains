#pragma once

#include "nui/vstacker.hpp"
#include "nui/grabbutton.hpp"
#include "nui/tabholder.hpp"
#include "scene/entity.hpp"
#include "dungeon/summary.hpp"
#include "tools/param.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace dungeon
{
    //! The sidebar shown in Dungeon Design state.

    class Sidebar final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        Sidebar();

        //! Destructor.
        ~Sidebar();

        //---------------------//
        //! @name Dungeon data
        //! @{

        //! The data of the dungeon to be read from.
        void useData(Data& data);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Refresh the tabs contents.
        void refreshTabContents();

        //! @}

    private:

        //! Holds informations about one tab content.
        struct TabContent
        {
            std::unique_ptr<nui::VStacker> stacker; //!< The stacker containing the tab content.
            std::vector<std::unique_ptr<nui::GrabButton>> buttons;  //!< The dynamic content for the tab.
        };

        //! The different tabs.
        enum TabsID
        {
            MONSTERS,
            TRAPS,
            FACILITIES,
            COUNT,
        };

    private:

        // Decorum
        sf::RectangleShape m_background;    //!< The background.

        // Stacker
        nui::VStacker m_globalStacker;  //!< Contains all elements.

        // Tabs
        nui::TabHolder m_tabs;                                  //!< The tabs.
        std::array<TabContent, TabsID::COUNT> m_tabContents;    //!< The contents for the tabs.

        // Ressources
        dungeon::Summary m_summary; //!< Shows the ressources.
    };
}
