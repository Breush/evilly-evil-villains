#include "states/hub/market.hpp"

#include "core/gettext.hpp"
#include "dungeon/databases/trapsdb.hpp"
#include "tools/platform-fixes.hpp"

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

    // Scroll area
    nuiRoot.attachChild(m_scrollArea);
    m_scrollArea.setContent(m_globalStacker);
    m_scrollArea.setRelativePosition({0.5f, 0.5f});
    m_scrollArea.setSize(nuiSize * sf::Vector2f{0.8f, 0.65f});
    m_scrollArea.centerOrigin();

    // Columns
    m_globalStacker.stackBack(m_columns[0u]);
    m_globalStacker.stackBack(m_columns[1u]);
    m_globalStacker.setPadding(0.f);
    m_columns[0u].setPadding(0.f);
    m_columns[1u].setPadding(0.f);

    refreshColumns();
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

//---------------//
//----- ICU -----//

void HubMarket::refreshColumns()
{
    m_trapLockers.clear();

    dungeon::TrapsDB trapsDB;
    const auto& trapsList = trapsDB.get();
    m_trapLockers.resize(trapsList.size());

    auto pTrapData = std::begin(trapsList);
    for (uint i = 0u; i < trapsList.size(); ++i, pTrapData++) {
        auto& trapLocker = m_trapLockers[i];
        trapLocker = std::make_unique<hub::TrapLocker>();
        trapLocker->setSource(pTrapData->first, pTrapData->second);
        trapLocker->setSize({0.4f * nuiLayer().size().x, 100.f});
        trapLocker->setLocked((i % 2u == 0u));
        m_columns[i % 2u].stackBack(*trapLocker);
    }
}
