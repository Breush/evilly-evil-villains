#include "states/game/dcb.hpp"

#include "core/gettext.hpp"
#include "dungeon/data.hpp"
#include "world/context.hpp"

using namespace states;

// TODO Make this part fun somehow with negociation for the dosh value...

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
    nuiRoot.attachChild(m_appreciationGauge);
    m_appreciationGauge.setLocalPosition({50, 50});
    m_appreciationGauge.setLimits(0.f, 534.f);
    m_appreciationGauge.setValue(42.f);

    // Buttons
    nuiRoot.attachChild(m_button);
    m_button.setAction(_("Create dungeon"), [this] { createDungeon(); });
    m_button.setLocalPosition(nuiSize / 2.f);
    m_button.setCentered(true);
}

void GameDCB::createDungeon()
{
    dungeon::Data dungeonData;
    const auto& worldInfo = world::context.selectedWorld();

    dungeonData.setName(worldInfo.name);
    dungeonData.setFloorsCount(7u);
    dungeonData.setRoomsByFloor(5u);
    dungeonData.createFiles(L"worlds/" + worldInfo.folder);

    stackClear(StateID::GAME_DUNGEON_DESIGN);
}
