#pragma once

#include "nui/uicore.hpp"
#include "nui/donjon/inter.hpp"
#include "states/state.hpp"

class GameDonjonDesignState : public State
{
public:
    GameDonjonDesignState(StateStack& stack);

    virtual void draw();
    virtual bool update(sf::Time dt);
    virtual bool handleEvent(const sf::Event& event);

private:
    // NUI
    nui::uiCore m_uiCore;
    nui::DonjonInter m_donjonInter;
};
