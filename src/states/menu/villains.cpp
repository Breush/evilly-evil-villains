#include "states/menu/villains.hpp"

#include "core/gettext.hpp"
#include "context/villains.hpp"

using namespace states;

MenuVillains::MenuVillains(StateStack& stack)
    : baseClass(stack)
{
    // Creating scene
    auto& nuiRoot = nuiLayer().root();
    const auto& nuiSize = nuiLayer().size();

    // Background
    nuiRoot.attachChild(m_background);
    m_background.setDepth(100.f);
    m_background.setFillColor(sf::Color(0, 0, 0, 230));
    m_background.setSize(nuiSize);

    // Title
    nuiRoot.attachChild(m_title);
    m_title.setText(_("Manage your villains"));
    m_title.setPrestyle(sfe::Label::Prestyle::MENU_TITLE);
    m_title.setRelativePosition({0.5f, 0.05f});
    m_title.centerOrigin();

    // List of villains
    nuiRoot.attachChild(m_list);
    m_list.setSize({0.45f * nuiSize.x, 0.7f * nuiSize.y});
    m_list.setRelativePosition({0.05f, 0.15f});
    m_list.setColumnsTitles({_("Villain")});

    // Back button
    nuiRoot.attachChild(m_backButton);
    m_backButton.setRelativeOrigin({0.5f, 1.f});
    m_backButton.setRelativePosition({0.5f, 0.95f});
    m_backButton.setAction(_("Back"), [this] { stackPop(); });

    // List
    context::villains.load();
    for (const auto& villain : context::villains.get())
        m_list.addLine({villain.name});
}

//------------------//
//----- Events -----//

bool MenuVillains::handleEvent(const sf::Event& event)
{
    // Escape quits current state
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPop();
        return false;
    }

    return State::handleEvent(event);
}
