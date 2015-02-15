#include "states/quit.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "tools/tools.hpp"

QuitState::QuitState(StateStack& stack)
    : State(stack)
{
    const auto& resolution = Application::context().resolution;

    // Background
    sceneLayer(Layers::NUI).attachChild(m_background);
    m_background.setFillColor(sf::Color(0, 0, 0, 230));
    m_background.setSize(Application::context().resolution);

    // Text
    sceneLayer(Layers::NUI).attachChild(m_question);
    m_question.setFont(Fonts::NUI);
    m_question.setText(_("Do you really want to quit?"));
    m_question.setCharacterSize(50);
    m_question.setPosition(0.5f * resolution.x, 0.4f * resolution.y);
    m_question.setCentered(true);

    // Buttons
    sceneLayer(Layers::NUI).attachChild(m_noButton);
    m_noButton.setAction(_("No, sorry, it is a mistake."), [this]() { stackPop(); });
    m_noButton.setLocalPosition(0.5f * resolution);
    m_noButton.setCentered(true);

    sceneLayer(Layers::NUI).attachChild(m_yesButton);
    m_yesButton.setAction(_("Yes, I will lose everything not saved."), [this]() { stackClear(); });
    m_yesButton.setLocalPosition({0.5f * resolution.x, 0.6f * resolution.y});
    m_yesButton.setCentered(true);
}

bool QuitState::handleEvent(const sf::Event& event)
{
    // Back to previous state on Escape
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPop();
        return false;
    }

    return State::handleEvent(event);
}

