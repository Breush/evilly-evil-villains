#include "states/menu/createvillain.hpp"

#include "core/gettext.hpp"
#include "context/villains.hpp"
#include "tools/tools.hpp"

using namespace states;

MenuCreateVillain::MenuCreateVillain(StateStack& stack)
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
    m_title.setText(_("How evil are you?"));
    m_title.setPrestyle(sfe::Label::Prestyle::MENU_TITLE);
    m_title.setRelativePosition({0.5f, 0.05f});
    m_title.centerOrigin();

    // Pop up
    nuiRoot.attachChild(m_popDialog);
    m_popDialog.setSize(nuiSize);

    // Form
    nuiRoot.attachChild(m_form);
    m_form.centerOrigin();
    m_form.setSize({500.f, 500.f});
    m_form.setRelativePosition({0.5f, 0.5f});

    // World name
    m_form.add(_("Evil name"), m_villainNameEntry);
    m_villainNameEntry.setLength(20u);

    // Buttons
    nuiRoot.attachChild(m_stacker);
    m_stacker.setRelativeOrigin({1.f, 0.5f});
    m_stacker.setRelativePosition({0.95f, 0.5f});

    for (auto& button : m_buttons)
        m_stacker.stackBack(button, nui::Align::OPPOSITE);

    m_buttons[0].setAction(_("Create!"), [this] { createVillain(); });
    m_buttons[1].setAction(_("Back"), [this] { stackPop(); });
}

//------------------//
//----- Events -----//

void MenuCreateVillain::handleEvent(const sf::Event& event)
{
    // Escape quits current state
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {

        // TODO Make PopDialog a state somehow?
        if (m_popDialog.opened()) m_popDialog.close();
        else stackPop();

        return;
    }

    State::handleEvent(event);
}

//-------------------------------//
//----- Villains management -----//

void MenuCreateVillain::createVillain()
{
    auto villainName = m_villainNameEntry.text();
    returnif (villainName.empty());

    // Check that villain name does not already exists
    if (context::villains.getFromVillainName(villainName) != nullptr) {
        m_popDialog.reset();
        m_popDialog.setTitle(_("Error"));
        m_popDialog.setMessage(_("There is already a villain with this name,\nand he is surely eviller than you."));
        m_popDialog.addButton(_("OK"), [this] { m_popDialog.close(); });
        m_popDialog.setExitCallback([this] { m_popDialog.close(); });
        m_popDialog.open();
        return;
    }

    context::villains.add(villainName);
    context::villains.save();

    stackPop();
}
