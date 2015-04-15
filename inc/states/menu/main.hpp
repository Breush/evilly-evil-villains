#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/choicebox.hpp"
#include "nui/reactimage.hpp"
#include "sfe/label.hpp"
#include "sfe/sprite.hpp"

#include <SFML/Graphics/Text.hpp>

namespace states
{
    class MenuMain : public State
    {
        using baseClass = State;

    public:
        MenuMain(StateStack& stack);
        virtual ~MenuMain() = default;
        StateID id() const noexcept override { return StateID::MENU_MAIN; }

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
        sfe::Label m_versionLabel;
        sfe::Label m_copyrightLabel;
    };
}
