#include "states/quit.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "tools/tools.hpp"

QuitState::QuitState(StateStack& stack)
    : State(stack)
    , m_uiCore(&m_mouseDetector)
{
    const auto& font = Application::context().fonts.get(Fonts::NUI);
    const auto& resolution = Application::context().resolution;

    // Background
    m_bg.setFillColor(sf::Color(0, 0, 0, 200));
    m_bg.setSize(Application::context().resolution);

    // Text
    m_text.setFont(font);
    m_text.setString(_("Do you really want to quit?"));
    m_text.setPosition(0.5f * resolution.x, 0.4f * resolution.y);
    auto bounds = m_text.getLocalBounds();
    m_text.setOrigin((bounds.left + bounds.width) / 2.f, (bounds.top + bounds.height) / 2.f);

    // Buttons
    m_uiCore.add(&m_noButton);
    m_noButton.setAction(_("No, sorry, it is a mistake."), [this]() { stackPop(); });
    m_noButton.setLocalPosition(0.5f * resolution);
    m_noButton.setOrigin(m_noButton.size() / 2.f);

    m_uiCore.add(&m_yesButton);
    m_yesButton.setAction(_("Yes, I will lose everything not saved."), [this]() { stackClear(); });
    m_yesButton.setLocalPosition({0.5f * resolution.x, 0.6f * resolution.y});
    m_yesButton.setOrigin(m_yesButton.size() / 2.f);
}

void QuitState::draw()
{
    auto& window = Application::context().window;
    window.setView(Application::context().views.get(Views::DEFAULT));

    window.draw(m_bg);
    window.draw(m_text);
    window.draw(m_uiCore);
}

bool QuitState::update(const sf::Time& dt)
{
    m_uiCore.update(dt);

    return false;
}

bool QuitState::handleEvent(const sf::Event& event)
{
    // Back to previous state on Escape
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape)
        stackPop();

    // Let ui core handle events
    m_uiCore.handleEvent(event);

    return false;
}

void QuitState::onHide()
{
    m_uiCore.forgetFocusedChild();
}

void QuitState::onShow()
{
    m_uiCore.rememberFocusedChild();
}

void QuitState::refresh()
{
    m_uiCore.refresh();
}

