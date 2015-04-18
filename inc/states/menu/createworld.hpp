#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "sfe/rectangleshape.hpp"
#include "sfe/label.hpp"
#include "nui/form.hpp"
#include "nui/textentry.hpp"
#include "nui/choicebox.hpp"
#include "nui/hstacker.hpp"
#include "nui/button.hpp"

#include <array>

namespace states
{
    class MenuCreateWorld : public State
    {
        using baseClass = State;

    public:
        MenuCreateWorld(StateStack& stack);
        virtual ~MenuCreateWorld() = default;
        StateID id() const noexcept override { return StateID::MENU_CREATEWORLD; }

        // Routines
        bool handleEvent(const sf::Event& event) override;

    protected:
        // Actions
        void createAndPlayWorld();

    private:
        // Background and title
        sfe::Label m_title;
        sfe::RectangleShape m_background;
        
        // NUI
        nui::Form m_form;
        nui::TextEntry m_worldNameEntry;
        nui::ChoiceBox m_villainBox;

        // Buttons
        nui::HStacker m_buttonsStacker;
        std::array<nui::Button, 2> m_buttons;
    };
}
