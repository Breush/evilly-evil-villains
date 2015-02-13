#pragma once

#include "states/state.hpp"
#include "scene/graph.hpp"
#include "nui/choicebox.hpp"
#include "nui/reactimage.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

class MenuMainState : public State
{
    typedef State baseClass;

public:
    MenuMainState(StateStack& stack);
    virtual ~MenuMainState() {}
    States::ID id() const override { return States::MENU_MAIN; }

    void draw() override;
    bool update(const sf::Time& dt) override;
    bool handleEvent(const sf::Event& event) override;

private:
    // NUI
    nui::ChoiceBox m_choiceBox;
    nui::ReactImage m_reactImage;

    // Choices possibilities
    std::vector<std::wstring> m_choices;

    // Background animation control
    sf::Sprite m_bgSprite;
    sf::Shader* m_bgShader;
    float      m_bgRotAngle;
};
