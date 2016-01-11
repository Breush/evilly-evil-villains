#include "states/quit.hpp"

#include "core/gettext.hpp"
#include "tools/tools.hpp"

using namespace states;

Quit::Quit(StateStack& stack)
    : State(stack)
{
    // Creating scene
    auto& nuiRoot = nuiLayer().root();
    const auto& nuiSize = nuiLayer().size();

    // Background
    nuiRoot.attachChild(m_background);
    m_background.setFillColor(sf::Color(0, 0, 0, 200));
    m_background.setSize(nuiSize);

    // Title
    nuiRoot.attachChild(m_title);
    m_title.setText(_("Do you really want to quit?"));
    m_title.setPrestyle(scene::Label::Prestyle::MENU_POPUP_TITLE);
    m_title.setRelativePosition({0.5f, 0.35f});
    m_title.centerOrigin();

    // Buttons
    nuiRoot.attachChild(m_noButton);
    m_noButton.set(_("No, sorry, it is a mistake."), [this]() { stackPop(); });
    m_noButton.setRelativePosition({0.5f, 0.5f});
    m_noButton.centerOrigin();

    nuiRoot.attachChild(m_yesButton);
    m_yesButton.set(_("Yes, I will lose everything not saved."), [this]() { stackClear(); });
    m_yesButton.setRelativePosition({0.5f, 0.6f});
    m_yesButton.centerOrigin();
}

//-------------------//
//----- Routine -----//

void Quit::handleEvent(const sf::Event& event)
{
    // Back to previous state on Escape
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPop();
        return;
    }

    State::handleEvent(event);
}

