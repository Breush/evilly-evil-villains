#pragma once

#include "scene/entity.hpp"
#include "nui/hstacker.hpp"
#include "nui/vstacker.hpp"
#include "scene/wrappers/rectangleshape.hpp"
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

        std::string _name() const final { return "nui::TabHolder"; }

        //------------------------//
        //! @name Tabs management
        //! @{

        //! Stack a tab to the end.
        void stackBack(std::wstring tooltipString, const std::string& textureID, scene::Entity& content);

        //! Selects a specific tab.
        void select(uint tabNumber);

        //! @}

        //----------------------------//
        //! @name Setters and getters
        //! @{

        //! Gets the size of the header part (no padding).
        sf::Vector2f headerSize() const;

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void onChildSizeChanges(scene::Entity& child) final;
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Called to recompute the size.
        void refreshSize();

        //! Refresh the global background.
        void refreshBackground();

        //! Refresh the selected tab content.
        void refreshContent();

        //! Refresh tabs position and background image.
        void refreshTabs();

        //! The selected tab has changed.
        void refreshSelectedTab();

        //! @}

    private:

        //! Contains a tab information.
        struct Tab
        {
            std::unique_ptr<sf::RectangleShape> image;      //!< The image.
            std::unique_ptr<sf::RectangleShape> background; //!< Its background.
            scene::Entity& content;                         //!< The entity to show when tab selected.
        };

    private:

        // Tabs
        std::vector<Tab> m_tabs;        //!< The tabs.
        uint m_selectedTab = -1u;       //!< The currently selected tab.

        // Content
        nui::VStacker m_globalStacker;  //!< Contains content.

        // Control over size
        float m_height; //!< Size override.

        // Decorum
        sf::RectangleShape m_background;    //!< The global background.

        float m_borderThick = 0.f;  //!< Border thickness.
        float m_vPadding = 0.f;     //!< Vertical padding.
        float m_tabSpacing = 0.f;   //!< Space between two tabs.
        float m_tabSize = 0.f;      //!< The forced size for a tab.
    };
}
