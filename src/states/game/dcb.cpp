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
    nuiRoot.attachChild(m_gaugesManager);
    m_gaugesManager.setRelativePosition({0.f, 0.80f});
    m_gaugesManager.setRelativeOrigin({0.f, 0.50f});

    // Message
    nuiRoot.attachChild(m_bubble);
    m_bubble.setRelativePosition({1.f, 0.f});
    m_bubble.setRelativeOrigin({1.f, 0.f});
    m_bubble.setSize(nuiSize / 3.f);
    /*m_bubble.forceMessage(_("Welcome to the Dungeon Community Bank (DCB). "
                            "I heard that you wanted to open your own dungeon, "
                            "this is the kind of operation we like to support here.\n\n"
                            "Let me just ask you: How do you want to name your tower of death?"));*/

    // Answer box
    nuiRoot.attachChild(m_answerBox);
    m_answerBox.setSize({0.75f * nuiSize.x, 0.40f * nuiSize.y});
    m_answerBox.setRelativePosition({0.25f, 0.60f});

    // Buttons
    nuiRoot.attachChild(m_button);
    m_button.setAction(_("Create dungeon"), [this] { createDungeon(); });
    m_button.setLocalPosition(nuiSize / 2.f);
    m_button.centerOrigin();

    // TODO Ask for dungeon name
    const auto& worldInfo = context::worlds.selected();
    m_controller.randomGaugesFromString(worldInfo.name);

    // TODO Get correct translation file
    m_controller.load("res/po/en_EN/dcb.xml");
}

void GameDCB::createDungeon()
{
    dungeon::Data dungeonData;
    const auto& worldInfo = context::worlds.selected();

    dungeonData.setName(worldInfo.name);
    dungeonData.setFloorsCount(7u);
    dungeonData.setRoomsByFloor(5u);
    dungeonData.createFiles(worldInfo.folder);

    stackClear(StateID::GAME_DUNGEON_DESIGN);
}
