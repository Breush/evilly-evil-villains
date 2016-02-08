#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/list.hpp"
#include "nui/hstacker.hpp"
#include "nui/pushbutton.hpp"
#include "scene/wrappers/rectanglegradient.hpp"
#include "scene/wrappers/label.hpp"

#include <array>

namespace states
{
    class MenuSelectWorld final : public State, public context::Interpreter
    {
        using baseClass = State;

    public:

        //! Constructor.
        MenuSelectWorld(StateStack& stack);

        //! Default destructor.
        ~MenuSelectWorld() = default;

        //----------------------//
        //! @name State control
        //! @{

        StateID id() const noexcept final { return StateID::MENU_SELECTWORLD; }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void handleEvent(const sf::Event& event) final;

        //! @}

        //--------------------//
        //! @name Interpreter
        //! @{

        inline std::wstring interpreterKey() const { return L"menuSelectWorld"; }
        context::CommandPtr interpret(std::vector<std::wstring>& tokens) final;
        void autoComplete(std::vector<std::wstring>& possibilities, const std::vector<std::wstring>& tokens, const std::wstring& lastToken) final;

        //! @}


        //---------------//
        //! @name Action
        //! @{

        //! Start game on selected world.
        void playOnSelectedWorld();

        //! @}

    private:

        // NUI
        nui::List m_list;                           //!< List of worlds.
        nui::HStacker m_stacker;                    //!< Stacker for buttons.
        std::array<nui::PushButton, 3u> m_buttons;  //!< Buttons Create/Back/Play.

        // Decorum
        scene::Label m_title;                   //!< Title.
        scene::RectangleGradient m_background;  //!< Background.
    };
}
