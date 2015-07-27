#pragma once

#include "scene/entity.hpp"
#include "nui/hstacker.hpp"
#include "nui/vstacker.hpp"
#include "sfe/rectangleshape.hpp"
#include "tools/param.hpp"

#include <vector>
#include <memory>
#include <functional>

namespace nui
{
    //! A UI element to create tabs.
    /*!
     *  The user creates any number of tabs,
     *  and affects one entity to each.
     *  The entity is shown below
     *  when the corresponding tab is selected.
     */

    class TabHolder final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        TabHolder();

        //! Default destructor.
        ~TabHolder() = default;

        //------------------------//
        //! @name Tabs management
        //! @{

        //! Stack a tab to the end.
        void stackBack(std::wstring tooltipString, TextureID textureID, scene::Entity& content);

        //! Selects a specific tab.
        void select(uint tabNumber);

        //! @}

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

        void handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Called to recompute the size.
        void refreshSize();

        //! Refresh the selected tab content.
        void refreshContent();

        //! @}

    private:

        //! Contains a tab information.
        struct Tab
        {
            std::unique_ptr<sfe::RectangleShape> image; //!< The image.
            std::wstring tooltipString;                 //!< The string to print for tooltip.
            scene::Entity& content;                     //!< The entity to show when tab selected.
        };

    private:

        // Tabs
        nui::HStacker m_tabsStacker;    //!< The stacker containing the tabs.
        std::vector<Tab> m_tabs;        //!< The tabs.
        Tab* m_selectedTab = nullptr;   //!< The currently selected tab.

        // Content
        nui::VStacker m_globalStacker;

        // Control over size.
        float m_height; //!< Size override.
    };
}
