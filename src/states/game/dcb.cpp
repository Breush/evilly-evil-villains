#include "states/game/dcb.hpp"

#include "core/gettext.hpp"
#include "dungeon/data.hpp"
#include "context/worlds.hpp"

using namespace states;

GameDCB::GameDCB(StateStack& stack)
    : baseClass(stack)
    , m_controller(m_gaugesManager, m_bubble, m_answerBox)
{
    // Creating scene
    auto& nuiRoot = nuiLayer().root();
    const auto& nuiSize = nuiLayer().size();

    // Background
    nuiRoot.attachChild(m_background);
    m_background.setFillColor({10, 10, 10});
    m_background.setSize(nuiSize);

    // Gauges
    m_gaugesManager.setRelativePosition({0.f, 0.80f});
    m_gaugesManager.setRelativeOrigin({0.f, 0.50f});

    // Message
    nuiRoot.attachChild(m_bubble);
    m_bubble.setRelativePosition({1.f, 0.f});
    m_bubble.setRelativeOrigin({1.f, 0.f});
    m_bubble.setSize(nuiSize / 3.f);
    m_bubble.forceMessage(_("Welcome to the Dungeon Community Bank (DCB). "
                            "I heard that you wanted to open your own dungeon, "
                            "this is the kind of operation we like to support here.\n\n"
                            "Let me just ask you: How do you want to name your tower of death?"));

    // Answer box
    nuiRoot.attachChild(m_answerBox);
    m_answerBox.setSize({0.75f * nuiSize.x, 0.40f * nuiSize.y});
    m_answerBox.setRelativePosition({0.25f, 0.60f});

    // Entry
    m_answerBox.attachChild(m_nameEntry);
    m_nameEntry.setLength(20u);
    m_nameEntry.setRelativePosition({0.5f, 0.5f});
    m_nameEntry.centerOrigin();
    m_nameEntry.setOnValidateCallback([this] { onNameValidate(); });

    // Button
    m_answerBox.attachChild(m_okButton);
    m_okButton.setAction(_("OK"), [this] { onNameValidate(); });
    m_okButton.setLocalPosition(m_nameEntry.localPosition() + sf::Vector2f{m_nameEntry.size().x / 2.f + 10.f, 0.f});
    m_okButton.setRelativeOrigin({0.f, 0.5f});

    // TODO Get correct translation file
    m_controller.load("res/po/en_EN/dcb.xml");
}

//-----------------------//
//----- Interaction -----//

void GameDCB::onNameValidate()
{
    returnif (m_nameEntry.text().empty());

    m_answerBox.detachChild(m_nameEntry);
    m_answerBox.detachChild(m_okButton);
    nuiLayer().root().attachChild(m_gaugesManager);

    m_controller.randomGaugesFromString(m_nameEntry.text());
    // TODO m_controller.startSequence();
}

void GameDCB::createDungeon()
{
    dungeon::Data dungeonData;
    const auto& worldInfo = context::worlds.selected();

    dungeonData.setName(m_nameEntry.text());
    dungeonData.setFloorsCount(7u);
    dungeonData.setRoomsByFloor(5u);
    dungeonData.createFiles(worldInfo.folder);

    stackClear(StateID::GAME_DUNGEON_DESIGN);
}
