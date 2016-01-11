#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/rangeselector.hpp"
#include "nui/scrollarea.hpp"
#include "nui/choicebox.hpp"
#include "nui/checkbox.hpp"
#include "nui/vstacker.hpp"
#include "nui/hstacker.hpp"
#include "nui/pushbutton.hpp"
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
            scene::Label title;                 //!< Area title.
            nui::VStacker stacker;              //!< Stacker for title and scroll area.
            nui::ScrollArea scrollArea;         //!< Area for configuration.
            nui::Form form;                     //!< The form inside the scroll area.
            nui::Frame frame;                   //!< Area delimiter.
        };

    private:

        // NUI
        std::array<Area, AreaID::COUNT> m_areas;    //!< All the areas of configuration.
        nui::HStacker m_buttonsStacker;             //!< Stacker for buttons.
        std::array<nui::PushButton, 2u> m_buttons;      //!< Buttons.

        // General
        nui::ChoiceBox m_languageBox;                   //!< Select the language.
        nui::RangeSelector m_uiSizeSelector;            //!< Size of the interface.
        nui::RangeSelector m_uiFontFactorSelector;      //!< Font factor of the interface.
        nui::RangeSelector m_scrollingFactorSelector;   //!< Scrolling speed.
        nui::RangeSelector m_zoomSpeedSelector;         //!< Zoom speed.

        // Graphics
        std::vector<Resolution> m_resolutions;      //!< Available resolutions.
        nui::ChoiceBox m_resolutionBox;             //!< Screen resolutions.
        nui::CheckBox m_fullscreenBox;              //!< Fullscreen switch.
        nui::CheckBox m_vsyncBox;                   //!< VSync switch.
        nui::RangeSelector m_antialiasingSelector;  //!< Antialiasing level selector.

        // Audio
        nui::RangeSelector m_globalVolumeSelector;  //!< Relative global volume selector.
        nui::RangeSelector m_musicVolumeSelector;   //!< Music volume selector.
        nui::RangeSelector m_soundVolumeSelector;   //!< Sound volume selector.

        // Dirty flags
        bool m_refreshBackgrounds = false;  //!< Should we refresh the background of areas?

        // Background and title
        scene::Label m_title;                   //!< The title of the menu page.
        scene::RectangleGradient m_background;  //!< The background of the menu page.
    };
}
