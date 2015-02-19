#pragma once

#include "states/state.hpp"
#include "nui/choicebox.hpp"
#include "nui/reactimage.hpp"
#include "sfe/sprite.hpp"

#include <SFML/Graphics/Text.hpp>

class MenuMainState : public State
{
    using baseClass = State;

public:
    MenuMainState(StateStack& stack);
    virtual ~MenuMainState() {}
    States::ID id() const override { return States::MENU_MAIN; }

    // Routines
    bool update(const sf::Time& dt) override;
    bool handleEvent(const sf::Event& event) override;

private:
    // NUI
    nui::ChoiceBox m_choiceBox;
    nui::ReactImage m_reactImage;

    // Choices possibilities
    std::vector<std::wstring> m_choices;

    // Decorum
    sfe::Sprite m_background;
};
