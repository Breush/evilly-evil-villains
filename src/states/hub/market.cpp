#include "states/hub/market.hpp"

#include "core/gettext.hpp"

using namespace states;

HubMarket::HubMarket(StateStack& stack)
    : baseClass(stack)
{
    // Creating scene
    auto& nuiRoot = nuiLayer().root();
    const auto& nuiSize = nuiLayer().size();

    // Background
    nuiRoot.attachChild(m_background);
    m_background.setDepth(100.f);
    m_background.setPointsColors({0u, 0u, 0u, 210u}, {0u, 0u, 0u, 255u});
    m_background.setSize(nuiSize);

    // Title
    nuiRoot.attachChild(m_title);
    m_title.setText(_("market"));
    m_title.setPrestyle(scene::Label::Prestyle::MENU_TITLE);
    m_title.setRelativePosition({0.5f, 0.05f});
    m_title.centerOrigin();

    // Stacker for buttons
    nuiRoot.attachChild(m_button);
    m_button.setRelativeOrigin({0.5f, 1.f});
    m_button.setRelativePosition({0.5f, 0.95f});
    m_button.setAction(_("Back"), [this] { stackPop(); });
}

//-------------------//
//----- Routine -----//

void HubMarket::handleEvent(const sf::Event& event)
{
    // Escape quits current state
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPop();
        return;
    }

    State::handleEvent(event);
}
