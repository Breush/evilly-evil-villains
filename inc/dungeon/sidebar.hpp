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
    //! The sidebar shown in Dungeon Design mode.

    class Sidebar final : public scene::Entity
    {
        using baseClass = scene::Entity;

        //! What tab content is currently shown.
        enum class Mode {
            MONSTERS,
            TRAPS,
            FACILITIES,
        };

    public:

        //! Constructor.
        Sidebar();

        //! Destructor.
        ~Sidebar();

        //---------------------//
        //! @name Mode
        //! @{

        //! Sets what tab content is currently shown.
        void setMode(Mode mode);

        //! @}

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
        nui::TabHolder m_tabs;              //!< The tabs.
        nui::VStacker m_tabContentStacker;  //!< The stacker containing the current tab content.
        std::vector<std::unique_ptr<nui::GrabButton>> m_tabContent; //!< The dynamic content for the tabs.

        // Ressources
        dungeon::Summary m_summary; //!< Shows the ressources.
    };
}
