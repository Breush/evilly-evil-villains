#pragma once

#include "scene/entity.hpp"
#include "nui/hstacker.hpp"
#include "sfe/rectangleshape.hpp"
#include "tools/param.hpp"

#include <vector>
#include <memory>
#include <functional>

namespace nui
{
    //! A tool to create tabs.

    class TabHolder final : public scene::Entity
    {
        using baseClass = scene::Entity;

        //! Callback type on selection.
        using SelectCallback = std::function<void()>;

    public:

        //! Constructor.
        TabHolder();

        //! Default destructor.
        ~TabHolder() = default;

        //------------------------//
        //! @name Tabs management
        //! @{

        //! Stack a tab to the end.
        void stack_back(std::wstring tooltipString, TextureID textureID, const SelectCallback& callback = nullptr);

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

        //! @}

    private:

        //! Contains a tab information.
        struct Tab
        {
            std::unique_ptr<sfe::RectangleShape> image; //!< The image.
            std::wstring tooltipString;                 //!< The string to print for tooltip.
            SelectCallback callback;                    //!< What to do on selection.
        };

    private:

        // Tabs
        nui::HStacker m_tabsStacker;    //!< The stacker containing the tabs.
        std::vector<Tab> m_tabs;        //!< The tabs.

        // Control over size.
        float m_height; //!< Size override.
    };
}
