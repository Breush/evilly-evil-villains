#pragma once

#include "nui/vstacker.hpp"
#include "nui/scrollarea.hpp"
#include "nui/grabbutton.hpp"
#include "nui/tabholder.hpp"
#include "scene/entity.hpp"
#include "dungeon/sidebar/summary.hpp"
#include "dungeon/sidebar/minimap.hpp"
#include "scene/wrappers/rectangleshape.hpp"
#include "tools/param.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

// Forward declarations

namespace scene
{
    class Scene;
    class Graph;
}

namespace dungeon
{
    class Data;
    class Inter;
    class MonsterCage;

    //! The sidebar shown in Dungeon Design state.

    class Sidebar final : public scene::Entity, private context::EventReceiver
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        Sidebar(scene::Graph& graph, Inter& inter);

        //! Destructor.
        ~Sidebar();

        //! Initialize after textures are loaded.
        void init();

        std::string _name() const final { return "dungeon::Sidebar"; }

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

        void refreshNUI(const config::NUIGuides& cNUI) final;
        void onSizeChanges() final;
        void onChildSizeChanges(scene::Entity& child) final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        void receive(const context::Event& event);

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Refresh the number of monsters cages.
        void refreshKnownMonsters();

        //! Refresh the monsters tab contents.
        void refreshTabMonstersContent();

        //! Refresh the traps tab contents.
        void refreshTabTrapsContent();

        //! Refresh the facilities tab contents.
        void refreshTabFacilitiesContent();

        //! Refresh the tools tab contents.
        void refreshTabToolsContent();

        //! Refresh the tabs contents.
        void refreshTabContents();

        //! Refresh the tab content size.
        void refreshTabParameters();

        //! Refreh the scroll areas size.
        void refreshScrollAreasSize();

        //! @}

    private:

        //! Holds informations about one tab content.
        struct TabContent
        {
            nui::ScrollArea scrollArea; //!< The scrollable area.
            nui::VStacker stacker;      //!< The stacker containing the tab content.
            std::vector<std::unique_ptr<nui::GrabButton>> buttons;  //!< The dynamic content for the traps and facilities.
            std::vector<std::unique_ptr<MonsterCage>> monsterCages; //!< The dynamic content for monsters.
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

        Inter& m_inter;         //!< Reference to the dungeon inter.
        Data* m_data = nullptr; //!< Reference to the whole data.
        scene::Scene& m_scene;  //!< The main scene, reference here for minimap interaction.

        // Stacker
        nui::VStacker m_globalStacker;  //!< Contains all elements.

        // Tabs
        nui::TabHolder m_tabHolder;                             //!< The tabs.
        std::array<TabContent, TabsID::COUNT> m_tabContents;    //!< The contents for the tabs.

        // Elements
        dungeon::Minimap m_minimap; //!< Shows the minimap of the dungeon.
        dungeon::Summary m_summary; //!< Shows the ressources.

        // Decorum
        scene::RectangleShape m_background; //!< The background.
        float m_vPadding = 0.f;             //!< Vertical padding.
        float m_borderThick = 0.f;          //!< Border thickness.
    };
}
