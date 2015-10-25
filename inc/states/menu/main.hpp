#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/choicebox.hpp"
#include "nui/reactimage.hpp"
#include "scene/wrappers/label.hpp"
#include "scene/wrappers/sprite.hpp"
#include "scene/wrappers/rectangleshape.hpp"

#include <SFML/Graphics/Text.hpp>

namespace states
{
    //! Main menu state.

    class MenuMain final : public State
    {
        using baseClass = State;

    public:

        //! Constructor.
        MenuMain(StateStack& stack);

        //! Destructor.
        ~MenuMain();

        //----------------------//
        //! @name State control
        //! @{

        StateID id() const noexcept final { return StateID::MENU_MAIN; }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        bool update(const sf::Time& dt) final;
        void handleEvent(const sf::Event& event) final;
        void refreshWindow(const config::WindowInfo& cWindow) final;

        //! @}

    private:

        // Fading
        scene::RectangleShape m_fadingRectangle;            //!< Smooth transition to the next state.
        sf::Time m_fadingTime = sf::Time::Zero;             //!< Current time for fading effect.
        const sf::Time m_fadingDelay = sf::seconds(1.75f);  //!< How much time to wait for fading.

        // NUI
        nui::ChoiceBox m_choiceBox;             //!< The box containing choices.
        nui::ReactImage m_reactImage;           //!< The reactive image.
        std::vector<std::wstring> m_choices;    //!< The list of choices.

        // Decorum
        scene::Sprite m_background;       //!< Background.
        scene::Label m_versionLabel;      //!< Version of the application.
        scene::Label m_copyrightLabel;    //!< Copyright info.
    };
}
