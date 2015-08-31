#include "states/menu/managevillains.hpp"

#include "core/gettext.hpp"
#include "context/villains.hpp"
#include "tools/tools.hpp"
#include "tools/string.hpp"

using namespace states;

MenuManageVillains::MenuManageVillains (StateStack& stack)
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

    // Pop up
    nuiRoot.attachChild(m_popDialog);
    m_popDialog.setSize(nuiSize);

    // List of villains
    nuiRoot.attachChild(m_list);
    m_list.setSize({0.45f * nuiSize.x, 0.7f * nuiSize.y});
    m_list.setRelativePosition({0.05f, 0.15f});
    m_list.setColumnsTitles({_("Villain"), _("Dosh"), _("Worlds")});
    m_list.setColumnAdapt(2u, nui::Adapt::FIT);
    m_list.setColumnAlign(2u, nui::Align::CENTER, nui::Align::CENTER);
    context::villains.load();
    refreshVillainsList();

    // Buttons
    nuiRoot.attachChild(m_stacker);
    m_stacker.setRelativeOrigin({1.f, 0.5f});
    m_stacker.setRelativePosition({0.95f, 0.5f});

    for (auto& button : m_buttons)
        m_stacker.stackBack(button, nui::Align::OPPOSITE);

    m_buttons[0].setAction(_("Create new Villain"), [this] { stackPush(StateID::MENU_CREATEVILLAIN); });
    m_buttons[1].setAction(_("Delete selected Villain"), [this] { deleteSelectedVillain(); });
    m_buttons[2].setAction(_("Back"), [this] { stackPop(); });
}

//-------------------//
//----- Routine -----//

void MenuManageVillains::handleEvent(const sf::Event& event)
{
    // Escape quits current state
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPop();
        return;
    }

    State::handleEvent(event);
}

//-------------------------------//
//----- Villains management -----//

void MenuManageVillains::deleteSelectedVillain()
{
    auto villain = m_list.selectedLine();
    returnif (villain == -1u);

    // Print message to confirm delete of all worlds.
    auto villainWorldsCount = context::villains.get(villain).worldsCount();
    if (villainWorldsCount != 0u) {
        m_popDialog.reset();
        m_popDialog.setTitle(_("Villain deletion"));
        m_popDialog.setMessage(_("Deleting a villain will DELETE all the WORLDS\nthe villain is in!"));
        m_popDialog.addButton(_("Cancel"), [this] { m_popDialog.close(); });
        m_popDialog.addButton(_("DELETE villain and his worlds"), [this, villain] { confirmDeletion(villain); });
        m_popDialog.setExitCallback([this] { m_popDialog.close(); });
        m_popDialog.open();
    }
    // No world associated, no need to confirm
    else {
        confirmDeletion(villain);
    }

}

void MenuManageVillains::confirmDeletion(uint villain)
{
    context::villains.remove(villain);
    context::villains.save();
    refreshVillainsList();
    m_popDialog.close();
}

void MenuManageVillains::refreshVillainsList() noexcept
{
    // Reload list
    m_list.clearLines();
    for (const auto& villain : context::villains.get())
        m_list.addLine({villain.name, toWString(villain.doshWallet.value()), toWString(villain.worldsCount())});
}
