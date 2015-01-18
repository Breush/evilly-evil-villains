#pragma once

#include "states/state.hpp"
#include "nui/uicore.hpp"
#include "nui/list.hpp"
#include "nui/hstacker.hpp"
#include "nui/button.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

class MenuSelectWorldState : public State
{
    typedef State baseClass;

public:
    MenuSelectWorldState(StateStack& stack, Context context);
    virtual ~MenuSelectWorldState();

    void draw() override;
    bool update(sf::Time dt) override;
    bool handleEvent(const sf::Event& event) override;

protected:
    void onShow() override;

private:
    // NUI
    nui::uiCore m_uiCore;
    nui::List m_list;
    nui::HStacker m_stacker;
    std::array<nui::Button*, 3> m_buttons;

    // Background
    sf::RectangleShape m_bgRect;
};

