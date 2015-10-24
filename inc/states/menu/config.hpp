#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/rangeselector.hpp"
#include "nui/scrollarea.hpp"
#include "nui/choicebox.hpp"
#include "nui/vstacker.hpp"
#include "nui/hstacker.hpp"
#include "nui/button.hpp"
#include "nui/form.hpp"
#include "scene/wrappers/label.hpp"
#include "scene/wrappers/rectangleshape.hpp"

namespace states
{
    //! The menu state to configure various options.

    class MenuConfig final : public State
    {
        using baseClass = State;

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

        bool update(const sf::Time& dt) final;
        void refreshNUI(const config::NUIGuides& cNUI) final;
        void refreshWindow(const config::WindowInfo& cWindow) final;
        void handleEvent(const sf::Event& event) final;

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
            scene::RectangleShape background;   //!< Area delimiter.
        };

    private:

        // NUI
        std::array<Area, AreaID::COUNT> m_areas;    //!< All the areas of configuration.
        nui::HStacker m_buttonsStacker;             //!< Stacker for buttons.
        std::array<nui::Button, 2u> m_buttons;      //!< Buttons.

        // Graphics
        nui::ChoiceBox m_fullscreenBox;             //!< Fullscreen switch.
        nui::ChoiceBox m_vsyncBox;                  //!< VSync switch.
        nui::RangeSelector m_antialiasingSelector;  //!< Antialiasing level selector.

        // Dirty flags
        bool m_refreshBackgrounds = false;  //!< Should we refresh the background of areas?

        // Background and title
        scene::Label m_title;               //!< The title of the menu page.
        scene::RectangleShape m_background; //!< The background of the menu page.
    };
}
