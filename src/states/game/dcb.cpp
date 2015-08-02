#include "states/game/dcb.hpp"

#include "core/gettext.hpp"
#include "dungeon/data.hpp"
#include "context/worlds.hpp"

using namespace states;

GameDCB::GameDCB(StateStack& stack)
    : baseClass(stack)
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
    m_gaugesManager.setLocalPosition({50, 50});

    // Buttons
    nuiRoot.attachChild(m_button);
    m_button.setAction(_("Create dungeon"), [this] { createDungeon(); });
    m_button.setLocalPosition(nuiSize / 2.f);
    m_button.centerOrigin();
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
