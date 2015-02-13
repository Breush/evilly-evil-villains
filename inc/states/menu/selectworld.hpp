#pragma once

#include "states/state.hpp"
#include "nui/list.hpp"
#include "nui/hstacker.hpp"
#include "nui/button.hpp"
#include "world/worldsdata.hpp"

#include <array>
#include <SFML/Graphics/RectangleShape.hpp>

class MenuSelectWorldState : public State
{
    typedef State baseClass;

public:
    MenuSelectWorldState(StateStack& stack);
    virtual ~MenuSelectWorldState() {}
    States::ID id() const override { return States::MENU_SELECTWORLD; }

    void draw() override;
    bool handleEvent(const sf::Event& event) override;

private:
    // NUI
    nui::List m_list;
    nui::HStacker m_stacker;
    std::array<nui::Button, 3> m_buttons;

    // Background
    sf::RectangleShape m_bgRect;
};

