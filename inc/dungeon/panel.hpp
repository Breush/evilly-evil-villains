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

        //--------------------------//
        //! @name Public properties
        //! @{

        //! Size override, as height is auto-controled.
        PARAMGSU(float, m_width, width, setWidth, refreshSize);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onChildSizeChanges(scene::Entity& child) final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        void receive(const Event& event) final;

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Called to recompute the size.
        void refreshSize();

        //! @}

    private:

        // Sidebar
        Sidebar& m_sidebar; //!< The sidebar, used as callback.

        // Tabs
        nui::HStacker m_tabsStacker;            //!< The stacker containing the tabs.
        std::array<nui::ImageButton, 3> m_tabs; //!< The tabs.

        // Control over size.
        float m_height; //!< Size override.
    };
}
