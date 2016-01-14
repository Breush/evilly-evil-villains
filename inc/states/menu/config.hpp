#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/dropdownlist.hpp"
#include "nui/scrollarea.hpp"
#include "nui/pushbutton.hpp"
#include "nui/checkbox.hpp"
#include "nui/hstacker.hpp"
#include "nui/slider.hpp"
#include "nui/frame.hpp"
#include "nui/form.hpp"
#include "scene/wrappers/label.hpp"
#include "scene/wrappers/rectangleshape.hpp"
#include "scene/wrappers/rectanglegradient.hpp"

namespace states
{
    //! The menu state to configure various options.

    class MenuConfig final : public State
    {
        using baseClass = State;

        //! How are represented resolutions.
        using Resolution = sf::Vector2u;

    public:

        //! Constructor.
        MenuConfig(StateStack& stack);

        //! Default destructor.
        ~MenuConfig() = default;

        //----------------------//
        //! @name State control
        //! @{

        StateID id() const noexcept final { return StateID::MENU_CONFIG; }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void handleEvent(const sf::Event& event) final;
        void refreshWindow(const config::WindowInfo& cWindow) final;

        //! @}

        //----------------------//
        //! @name Configuration
        //! @{

        //! Refresh the forms to current config.
        void refreshFormsFromConfig();

        //! Apply the last changes.
        void applyChanges();

        //! @}

        //! The different sections of the screen.
        enum AreaID
        {
            GENERAL,
            GRAPHICS,
            AUDIO,
            COUNT,  // Keep last.
        };

        //! Area parts.
        struct Area
        {
            nui::Frame frame;           //!< Area delimiter.
            nui::ScrollArea scrollArea; //!< Area for configuration.
            nui::Form form;             //!< The form inside the scroll area.
        };

    private:

        // NUI
        std::array<Area, AreaID::COUNT> m_areas;    //!< All the areas of configuration.
        nui::HStacker m_buttonsStacker;             //!< Stacker for buttons.
        std::array<nui::PushButton, 2u> m_buttons;      //!< Buttons.

        // General
        nui::DropDownList m_languageList;       //!< Select the language.
        nui::Slider m_uiSizeSlider;             //!< Size of the interface.
        nui::Slider m_uiFontFactorSlider;       //!< Font factor of the interface.
        nui::Slider m_scrollingFactorSlider;    //!< Scrolling speed.
        nui::Slider m_zoomSpeedSlider;          //!< Zoom speed.

        // Graphics
        std::vector<Resolution> m_resolutions;  //!< Available resolutions.
        nui::DropDownList m_resolutionList;     //!< Screen resolutions.
        nui::CheckBox m_fullscreenBox;          //!< Fullscreen switch.
        nui::CheckBox m_vsyncBox;               //!< VSync switch.
        nui::Slider m_antialiasingSlider;       //!< Antialiasing level selector.

        // Audio
        nui::Slider m_globalVolumeSlider;   //!< Relative global volume selector.
        nui::Slider m_musicVolumeSlider;    //!< Music volume selector.
        nui::Slider m_soundVolumeSlider;    //!< Sound volume selector.

        // Background and title
        scene::Label m_title;                   //!< The title of the menu page.
        scene::RectangleGradient m_background;  //!< The background of the menu page.
    };
}
