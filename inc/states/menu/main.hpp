#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/choicebox.hpp"
#include "nui/reactimage.hpp"
#include "scene/wrappers/label.hpp"
#include "scene/wrappers/sprite.hpp"

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

        //! Default destructor.
        ~MenuMain() = default;

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

        //! @}

    private:

        // NUI
        nui::ChoiceBox m_choiceBox;             //!< The box containing choices.
        nui::ReactImage m_reactImage;           //!< The reactive image.
        std::vector<std::wstring> m_choices;    //!< The list of choices.

        // Decorum
        sfe::Sprite m_background;       //!< Background.
        sfe::Label m_versionLabel;      //!< Version of the application.
        sfe::Label m_copyrightLabel;    //!< Copyright info.
    };
}
