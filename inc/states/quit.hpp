#pragma once

#include "states/state.hpp"
#include "nui/uicore.hpp"
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
    bool update(const sf::Time& dt) override;
    bool handleEvent(const sf::Event& event) override;
    void refresh() override;

    void onHide() override;
    void onShow() override;

private:
    sf::RectangleShape m_bg;
    sf::Text m_text;

    // NUI
    interaction::MouseDetector m_mouseDetector;
    nui::uiCore m_uiCore;
    nui::Button m_noButton;
    nui::Button m_yesButton;
};

