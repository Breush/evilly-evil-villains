#pragma once

#include "states/state.hpp"
#include "nui/button.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

class QuitState : public State
{
public:
    QuitState(StateStack& stack);
    virtual ~QuitState() {}
    virtual States::ID id() const override { return States::QUIT; }

    void draw() override;
    bool handleEvent(const sf::Event& event) override;

private:
    // Decorum
    sf::RectangleShape m_bg;
    sf::Text m_text;

    // NUI
    nui::Button m_noButton;
    nui::Button m_yesButton;
};

