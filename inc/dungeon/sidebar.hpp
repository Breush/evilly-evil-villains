#pragma once

#include "nui/vstacker.hpp"
#include "nui/scrollarea.hpp"
#include "nui/grabbutton.hpp"
#include "nui/tabholder.hpp"
#include "scene/entity.hpp"
#include "dungeon/summary.hpp"
#include "dungeon/minimap.hpp"
#include "dungeon/monsters.hpp"
#include "tools/param.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

// Forward declarations

namespace scene
{
    class Scene;
}

namespace dungeon
{
    //! The sidebar shown in Dungeon Design state.

    class Sidebar final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        Sidebar(scene::Scene& inScene);

        //! Destructor.
        ~Sidebar();

        //---------------------//
        //! @name Dungeon data
        //! @{

        //! The data of the dungeon to be read from.
        void useData(Data& data);

        //! @}

        //----------------//
        //! @name Minimap
        //! @{

        //! Sets the layer shown in minimap.
        inline void setMinimapLayer(scene::Layer& layer) { m_minimap.setLayer(layer); }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void refreshDisplay() final;
        void onSizeChanges() final;
        void onChildSizeChanges(scene::Entity& child) final;

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Refresh the tabs contents.
        void refreshTabContents();

        //! Refreh the scroll areas size.
        void refreshScrollAreasSize();

        //! @}

    private:

        //! Holds informations about one tab content.
        struct TabContent
        {
            nui::ScrollArea scrollArea; //!< The scrollable area.
            nui::VStacker stacker;      //!< The stacker containing the tab content.
            std::vector<std::unique_ptr<nui::GrabButton>> buttons;  //!< The dynamic content for the tab.
            std::vector<std::unique_ptr<MonsterCage>> monsterCage;  //!< The dynamic content for monsters.
        };

        //! The different tabs.
        enum TabsID
        {
            MONSTERS,
            TRAPS,
            FACILITIES,
            TOOLS,
            COUNT,
        };

    private:

        scene::Scene& m_scene;  //!< The main scene, reference here for minimap interaction.

        // Stacker
        nui::VStacker m_globalStacker;  //!< Contains all elements.

        // Tabs
        nui::TabHolder m_tabHolder;                             //!< The tabs.
        std::array<TabContent, TabsID::COUNT> m_tabContents;    //!< The contents for the tabs.

        // Elements
        dungeon::Minimap m_minimap; //!< Shows the minimap of the dungeon.
        dungeon::Summary m_summary; //!< Shows the ressources.
    };
}
