#include "dungeon/inter.hpp"

#include "core/gettext.hpp"
#include "core/application.hpp"
#include "context/villains.hpp"
#include "dungeon/traps/maker.hpp"
#include "dungeon/monsters/maker.hpp"
#include "dungeon/facilities/maker.hpp"
#include "tools/debug.hpp"
#include "tools/event.hpp"
#include "tools/tools.hpp"
#include "tools/platform-fixes.hpp" // make_unique

#include <SFML/Graphics/RenderWindow.hpp>
#include <sstream>

using namespace dungeon;

Inter::Inter(nui::ContextMenu& contextMenu)
    : m_contextMenu(contextMenu)
{
    // Grid
    m_grid.setVisible(false);
    addPart(&m_grid);

    // Spinbox
    attachChild(m_treasureEditSpinBox);
    m_treasureEditSpinBox.setDepth(-1.f);
    m_treasureEditSpinBox.setVisible(false);
    m_treasureEditSpinBox.setStep(10u);

    // Outer walls
    m_outerWalls[0].setTexture(&Application::context().textures.get("dungeon/inter/outer_wall_west"));
    m_outerWalls[1].setTexture(&Application::context().textures.get("dungeon/inter/outer_wall_east"));
    addPart(&m_outerWalls[0]);
    addPart(&m_outerWalls[1]);

    // Ref size
    m_refRoomSize = sf::v2f(Application::context().textures.get("dungeon/inter/inner_wall").getSize());
}

//-------------------//
//----- Routine -----//

void Inter::updateRoutine(const sf::Time&)
{
    // Do delayed refresh
    for (const auto& tileRefresh : m_tileRefreshPending)
        tileRefresh();
    m_tileRefreshPending.clear();
}

void Inter::onSizeChanges()
{
    m_grid.setSize(size());
    refreshOuterWalls();

    returnif (m_data == nullptr);
    // TODO OPTIM Well, it's just position/size of elements that need to be updated, right?
    refreshTiles();
    refreshMonsters();
}

//------------------//
//----- Events -----//

void Inter::handleGlobalEvent(const sf::Event& event)
{
    // Remove selection if something is clicked anywhere
    if (event.type == sf::Event::MouseButtonPressed)
        deselectTile();

#if DEBUG_GLOBAL > 0
    // Keyboard event
    if (event.type == sf::Event::KeyPressed) {
        // Control rows
        if (event.key.code == sf::Keyboard::Add)
            adaptFloorsCount(1);
        else if (event.key.code == sf::Keyboard::Subtract)
            adaptFloorsCount(-1);

        // Control columns
        else if (event.key.code == sf::Keyboard::Multiply)
            adaptRoomsByFloor(1);
        else if (event.key.code == sf::Keyboard::Divide)
            adaptRoomsByFloor(-1);

        // Dosh control
        else if (event.key.code == sf::Keyboard::Numpad9)
            m_data->villain().doshWallet.add(500);
        else if (event.key.code == sf::Keyboard::Numpad3)
            m_data->villain().doshWallet.sub(500);
    }

    refreshOuterWalls();
#endif
}

void Inter::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    // Selected the tile below
    selectTile(mousePos);

    // Harvest the money
    if (button == sf::Mouse::Left)
        harvestTileDosh(m_selectedTile->coords);

    // Do nothing in invasion
    if (m_invasion) {
        deselectTile();
        return;
    }

    // Remove spinbox interface if any
    m_treasureEditSpinBox.markForVisible(false);

    // Pop the context menu up
    if (button == sf::Mouse::Right)
        showTileContextMenu(m_selectedTile->coords, nuiPos);
}

void Inter::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    returnif (m_invasion);

    auto coords = tileFromLocalPosition(mousePos);
    setHoveredTile(coords);
}

void Inter::handleMouseLeft()
{
    returnif (m_invasion);

    resetHoveredTile();
}

//--------------------------//
//----- Dungeon events -----//

void Inter::receive(const Event& event)
{
    sf::Vector2u coords;

    switch (event.type)
    {
    case EventType::ROOM_DESTROYED:
    case EventType::ROOM_CONSTRUCTED:
        coords = {event.room.x, event.room.y};
        m_tileRefreshPending.emplace_back([=]() { return refreshTile(coords); });
        m_tileRefreshPending.emplace_back([=]() { return refreshNeighboursLayers(coords); });
        break;

    case EventType::FACILITY_CHANGED:
        coords = {event.facility.room.x, event.facility.room.y};
        m_tileRefreshPending.emplace_back([=]() { return refreshTileFacilities(coords); });
        m_tileRefreshPending.emplace_back([=]() { return refreshTileDoshLabel(coords); });
        // TODO That's only because of ladder...
        m_tileRefreshPending.emplace_back([=]() { return refreshNeighboursFacilities(coords); });
        break;

    case EventType::TRAP_CHANGED:
        coords = {event.room.x, event.room.y};
        m_tileRefreshPending.emplace_back([=]() { return refreshTileTraps(coords); });
        break;

    case EventType::MONSTER_ADDED:
        m_tileRefreshPending.emplace_back([=]() { return refreshMonsters(); });
        break;

    case EventType::MONSTER_EXPLODES_ROOM:
        coords = {event.action.room.x, event.action.room.y};
        m_data->destroyRoom(coords, true);
        m_data->graph().reconstructFromData();
        // TODO What happens if we destroy entrance?
        m_tileRefreshPending.emplace_back([=]() { return refreshMonsters(); });
        break;

    case EventType::HARVESTABLE_DOSH_CHANGED:
        coords = {event.room.x, event.room.y};
        m_tileRefreshPending.emplace_back([=]() { return refreshTileDoshLabel(coords); });
        break;

    case EventType::MODE_CHANGED:
        m_treasureEditSpinBox.markForVisible(false);
        m_invasion = (event.mode == Mode::INVASION);
        refreshMonstersActivity();
        deselectTile();
        break;

    default:
        break;
    }
}

//------------------------//
//----- Dungeon data -----//

void Inter::useData(Data& data)
{
    m_data = &data;
    setEmitter(&data);
    refreshFromData();
}

void Inter::refreshFromData()
{
    returnif (m_data == nullptr);

    const auto& roomsByFloor = m_data->roomsByFloor();
    const auto& floorsCount = m_data->floorsCount();

    // Grid
    m_grid.setRowsColumns(floorsCount, roomsByFloor);

    // Room tiles
    clearTiles();

    for (uint floor = 0u; floor < floorsCount; ++floor)
    for (uint room = 0u; room < roomsByFloor; ++room) {
        Tile tile;
        tile.coords = {floor, room};
        tile.room = &m_data->room({floor, room});
        m_tiles[{floor, room}] = std::move(tile);
    }

    // Sets the new size
    refreshSize();
    refreshTiles();
    refreshMonsters();
}

//---------------------------//
//----- Tile management -----//

sf::Vector2f Inter::tileLocalPosition(const sf::Vector2u& coords) const
{
    return m_grid.cellPosition(m_data->floorsCount() - coords.x - 1u, coords.y);
}

sf::Vector2u Inter::tileFromLocalPosition(const sf::Vector2f& pos)
{
    auto coords = m_grid.rowColumnFromCoords(pos);
    coords.x = m_data->floorsCount() - coords.x - 1u;
    return coords;
}

void Inter::addLayer(const sf::Vector2u& coords, const std::string& textureID)
{
    auto& tile = m_tiles.at(coords);

    // We don't know how the std::vector will act
    // when increasing its capacity, so it is better to remove
    // all pending references and pointers before
    for (auto& layer : tile.layers)
        removePart(&layer);

    tile.layers.emplace_back();

    auto& layer = tile.layers.back();
    layer.setTexture(&Application::context().textures.get(textureID));
    layer.setPosition(tileLocalPosition(coords));
    layer.setSize(tileSize());

    // And afterward, re-add all references/pointers
    // TODO Layers should be children, for easy z-depth.
    for (auto& layer : tile.layers)
        addPart(&layer);
}

void Inter::clearLayers(const sf::Vector2u& coords)
{
    auto& tile = m_tiles.at(coords);

    for (auto& layer : tile.layers)
        removePart(&layer);

    tile.layers.clear();
}

void Inter::clearTiles()
{
    for (auto& tile : m_tiles)
        clearLayers(tile.second.coords);

    m_tiles.clear();
    m_selectedTile = nullptr;
    m_hoveredTile = nullptr;
}

//-------------------------//
//----- Selected tile -----//

void Inter::selectTile(const sf::Vector2f& pos)
{
    selectTile(tileFromLocalPosition(pos));
}

void Inter::selectTile(const sf::Vector2u& coords)
{
    deselectTile();
    m_selectedTile = &m_tiles.at(coords);

    // TODO Have a NUI_SELECT shader
    for (auto& layer : m_selectedTile->layers)
        setPartShader(&layer, "nui/hover");
}

void Inter::deselectTile()
{
    returnif(m_selectedTile == nullptr);

    for (auto& layer : m_selectedTile->layers)
        resetPartShader(&layer);

    m_selectedTile = nullptr;
}

//------------------------//
//----- Hovered tile -----//

void Inter::setHoveredTile(const sf::Vector2u& coords)
{
    resetHoveredTile();
    m_hoveredTile = &m_tiles.at(coords);

    returnif(m_hoveredTile == m_selectedTile);

    for (auto& layer : m_hoveredTile->layers)
        setPartShader(&layer, "nui/hover");
}

void Inter::resetHoveredTile()
{
    returnif(m_hoveredTile == nullptr);
    returnif(m_hoveredTile == m_selectedTile);

    for (auto& layer : m_hoveredTile->layers)
        resetPartShader(&layer);

    m_hoveredTile = nullptr;
}

//------------------------//
//----- Context menu -----//

void Inter::addFacilityChoice(const sf::Vector2u& coords, const std::wstring& facilityID, const std::wstring& facilityName)
{
    // The facility is already there
    if (hasOfType(m_data->room(coords).facilities, facilityID)) {
        // TODO Add gain when facilities are in same model than traps
        std::wstring text = _("Remove") + L" " + facilityName;
        m_contextMenu.addChoice(text, [=]() { m_data->removeRoomFacility(coords, facilityID); });
    }

    // The facility is not there
    else {
        std::wstring text = _("Create") + L" " + facilityName + L" (-" + toWString(facilities::onCreateCost(facilityID)) + L"d)";
        m_contextMenu.addChoice(text, [=]() { m_data->createRoomFacility(coords, facilityID); });
    }
}

void Inter::showTileContextMenu(const sf::Vector2u& coords, const sf::Vector2f& nuiPos)
{
    auto& room = m_data->room(coords);
    m_contextMenu.clearChoices();

    // Context title
    std::wstringstream roomName;
    roomName << _("Room") << L' ' << coords.x << L'/' << coords.y;
    m_contextMenu.setTitle(roomName.str());

    // Room does not exists yet
    if (room.state == Data::RoomState::VOID) {
        std::wstring text = _("Build room") + L" (-" + toWString(m_data->onConstructRoomCost) + L"d)";
        if (m_data->onConstructRoomCost <= m_data->villain().doshWallet.value())
            m_contextMenu.addChoice(text, [=]() { m_data->constructRoom(coords); });
        else
            m_contextMenu.addChoice(text, nullptr);
    }

    // Room does exists
    else {
        // TODO Show really how much we get for destroying (adding all elements inside)
        m_contextMenu.addChoice(_("Destroy room") + L" (+" + toWString(m_data->onDestroyRoomGain) + L"d)", [=]() {
            m_data->destroyRoom(coords);
        });

        // Facilities
        addFacilityChoice(coords, L"ladder", _("ladder"));
        addFacilityChoice(coords, L"entrance", _("entrance"));

        // If treasure, modifiy pop-up
        if (hasOfType(m_data->room(coords).facilities, L"treasure"))
            m_contextMenu.addChoice(_("Edit treasure dosh"), [=]() { showEditTreasureDialog(coords); });
    }

    // Context positions
    m_contextMenu.setLocalPosition(nuiPos);
    m_contextMenu.markForVisible(true);
}

void Inter::showEditTreasureDialog(const sf::Vector2u& coords)
{
    auto& treasureData = *findOfType(m_data->room(coords).facilities, L"treasure");
    auto& treasureDosh = treasureData[L"dosh"].as_uint32();

    m_treasureEditSpinBox.entry().giveFocus();
    m_treasureEditSpinBox.setVisible(true);
    m_treasureEditSpinBox.set(treasureDosh);
    m_treasureEditSpinBox.setLocalPosition(tileLocalPosition(coords));
    m_treasureEditSpinBox.setMaxLimit(treasureDosh + m_data->villain().doshWallet.value());
    m_treasureEditSpinBox.setCallback([this, &coords, &treasureData] (uint32 oldValue, uint32 newValue) {
        // Sub or add
        if (newValue >= oldValue) m_data->villain().doshWallet.sub(newValue - oldValue);
        else m_data->villain().doshWallet.add(oldValue - newValue);
        treasureData[L"dosh"].as_uint32() = newValue;

        // Global dosh changed
        emitter()->emit(EventType::DOSH_CHANGED);

        // Treasure dosh changed
        Event event;
        event.type = EventType::FACILITY_CHANGED;
        event.facility.room = {coords.x, coords.y};
        emitter()->emit(event);
    });
}

//---------------------//
//----- Structure -----//

void Inter::setRoomWidth(const float roomWidth)
{
    // Note: We want room to keep the same ratio as original image.
    const float scaleFactor = roomWidth / m_refRoomSize.x;
    m_roomScale = {scaleFactor, scaleFactor};
    refreshSize();
}

void Inter::createRoomFacility(const sf::Vector2f& relPos, const std::wstring& facilityID)
{
    m_data->createRoomFacility(tileFromLocalPosition(relPos), facilityID);
}

void Inter::setRoomTrap(const sf::Vector2f& relPos, const std::wstring& trapID)
{
    m_data->setRoomTrap(tileFromLocalPosition(relPos), trapID);
}

void Inter::addMonster(const sf::Vector2f& relPos, const std::wstring& monsterID)
{
    m_data->addMonster(tileFromLocalPosition(relPos), monsterID);
}

void Inter::constructRoom(const sf::Vector2f& relPos)
{
    m_data->constructRoom(tileFromLocalPosition(relPos));
}

void Inter::destroyRoom(const sf::Vector2u& coords)
{
    m_data->destroyRoom(coords);
}

void Inter::removeRoomTrap(const sf::Vector2f& relPos)
{
    m_data->removeRoomTrap(tileFromLocalPosition(relPos));
}

void Inter::removeRoomFacilities(const sf::Vector2f& relPos)
{
    m_data->removeRoomFacilities(tileFromLocalPosition(relPos));
}

void Inter::adaptFloorsCount(int relativeValue)
{
    setFloorsCount(m_data->floorsCount() + relativeValue);
}

void Inter::adaptRoomsByFloor(int relativeValue)
{
    setRoomsByFloor(m_data->roomsByFloor() + relativeValue);
}

void Inter::setFloorsCount(uint value)
{
    m_data->setFloorsCount(value);
    refreshFromData();
}

void Inter::setRoomsByFloor(uint value)
{
    m_data->setRoomsByFloor(value);
    refreshFromData();
}

//----------------//
//----- Dosh -----//

void Inter::harvestTileDosh(const sf::Vector2u& coords)
{
    auto& trap = m_tiles[coords].trap;
    returnif (trap == nullptr);

    auto harvestableDosh = trap->harvestableDosh();
    if (harvestableDosh > 0u)
        m_data->villain().doshWallet.add(trap->harvestDosh());

    refreshTileDoshLabel(coords);
}

void Inter::configureDoshLabel(std::unique_ptr<sfe::Label>& doshLabel, const uint dosh, const sf::Color& color)
{
    // Nothing if no dosh
    if (dosh == 0u) {
        doshLabel = nullptr;
        return;
    }

    // Create label if does not exists
    if (doshLabel == nullptr) {
        doshLabel = std::make_unique<sfe::Label>();
        doshLabel->setPrestyle(sfe::Label::Prestyle::NUI);
        doshLabel->setColor(color);
        attachChild(*doshLabel);
    }

    // Re-format string
    std::wstringstream str;
    str << dosh << L'd';
    doshLabel->setText(str.str());
}

//-----------------------------------//
//----- Internal change updates -----//

void Inter::refreshSize()
{
    returnif (m_data == nullptr);

    const auto& roomsByFloor = m_data->roomsByFloor();
    const auto& floorsCount = m_data->floorsCount();
    const auto roomSize = m_roomScale * m_refRoomSize;
    setSize({roomSize.x * roomsByFloor, roomSize.y * floorsCount});
}

void Inter::refreshMonsters()
{
    m_monsters.clear();

    for (auto& monsterInfo : m_data->monstersInfo()) {
        auto monster = monsters::make(monsterInfo.coords, monsterInfo.data, *this);
        monster->setEmitter(m_data);
        monster->useGraph(m_data->graph());
        m_monsters.emplace_back(std::move(monster));
        attachChild(*m_monsters.back());
    }

    refreshMonstersActivity();
}

void Inter::refreshMonstersActivity()
{
    for (auto& monster : m_monsters)
        monster->setActive(m_invasion);
}

void Inter::refreshTiles()
{
    for (auto& tile : m_tiles)
        refreshTile(tile.second.coords);
}

void Inter::refreshTile(const sf::Vector2u& coords)
{
    returnif (coords.x >= m_data->floorsCount());
    returnif (coords.y >= m_data->roomsByFloor());

    refreshTileDoshLabel(coords);
    refreshTileLayers(coords);
    refreshTileTraps(coords);
    refreshTileFacilities(coords);
}

void Inter::refreshNeighboursLayers(const sf::Vector2u& coords)
{
    refreshTileLayers(m_data->roomNeighbourCoords(coords, Data::WEST));
    refreshTileLayers(m_data->roomNeighbourCoords(coords, Data::SOUTH));
    refreshTileLayers(m_data->roomNeighbourCoords(coords, Data::EAST));
    refreshTileLayers(m_data->roomNeighbourCoords(coords, Data::NORTH));
}

void Inter::refreshNeighboursFacilities(const sf::Vector2u& coords)
{
    refreshTileFacilities(m_data->roomNeighbourCoords(coords, Data::WEST));
    refreshTileFacilities(m_data->roomNeighbourCoords(coords, Data::SOUTH));
    refreshTileFacilities(m_data->roomNeighbourCoords(coords, Data::EAST));
    refreshTileFacilities(m_data->roomNeighbourCoords(coords, Data::NORTH));
}

void Inter::refreshTileDoshLabel(const sf::Vector2u& coords)
{
    returnif (coords.x >= m_data->floorsCount());
    returnif (coords.y >= m_data->roomsByFloor());

    const auto& room = m_data->room(coords);
    const auto& localPosition = tileLocalPosition(coords);
    auto& tile = m_tiles[coords];

    // Remove label if no room
    if (room.state == Data::RoomState::VOID) {
        tile.totalDoshLabel = nullptr;
        tile.harvestableDoshLabel = nullptr;
        return;
    }

    // Harvestable dosh
    uint harvestableDosh = 0u;
    if (tile.trap != nullptr)
        harvestableDosh += tile.trap->harvestableDosh();
    configureDoshLabel(tile.harvestableDoshLabel, harvestableDosh, sf::Color::Red);

    // Total dosh
    uint totalDosh = 0u;
    totalDosh += harvestableDosh;
    totalDosh += m_data->roomTreasureDosh(coords);
    configureDoshLabel(tile.totalDoshLabel, totalDosh, sf::Color::White);

    // Re-position
    if (tile.totalDoshLabel != nullptr)
        tile.totalDoshLabel->setLocalPosition(localPosition);

    if (tile.harvestableDoshLabel != nullptr) {
        tile.harvestableDoshLabel->setLocalPosition(localPosition + tileSize());
        tile.harvestableDoshLabel->setRelativeOrigin({1.f, 1.f});
    }
}

void Inter::refreshTileLayers(const sf::Vector2u& coords)
{
    returnif (coords.x >= m_data->floorsCount());
    returnif (coords.y >= m_data->roomsByFloor());

    const auto& room = m_data->room(coords);
    const auto state = room.state;

    // Reset
    clearLayers(coords);

    // Room is somehow bugged
    if (state == Data::RoomState::UNKNOWN) {
        std::cerr << "/!\\ Found a room with unknown state at " << coords << "." << std::endl;
        std::cerr << "If that is a recurrent issue, please report this bug." << std::endl;
        addLayer(coords, "default");
        return;
    }

    // Room is not constructed
    if (state == Data::RoomState::VOID)
    {
        addLayer(coords, "dungeon/inter/void_room");

        if (m_data->isRoomConstructed(m_data->roomNeighbourCoords(coords, Data::WEST)))
            addLayer(coords, "dungeon/inter/void_west_transition");
        if (m_data->isRoomConstructed(m_data->roomNeighbourCoords(coords, Data::SOUTH)))
            addLayer(coords, "dungeon/inter/void_south_transition");
        if (m_data->isRoomConstructed(m_data->roomNeighbourCoords(coords, Data::EAST)))
            addLayer(coords, "dungeon/inter/void_east_transition");

        return;
    }

    // Add room textures
    addLayer(coords, "dungeon/inter/inner_wall");
    addLayer(coords, "dungeon/inter/floor");

    if (!m_data->isRoomConstructed(m_data->roomNeighbourCoords(coords, Data::EAST)))
        addLayer(coords, "dungeon/inter/right_wall");
}

void Inter::refreshTileFacilities(const sf::Vector2u& coords)
{
    returnif (coords.x >= m_data->floorsCount());
    returnif (coords.y >= m_data->roomsByFloor());

    auto& room = m_data->room(coords);
    auto& tile = m_tiles[coords];

    // Reset
    tile.facilities.clear();

    // Room is not constructed
    returnif (room.state != Data::RoomState::CONSTRUCTED);

    // If room below has a ladder, we need a way down
    bool needWayDown = m_data->roomNeighbourAccessible(coords, Data::Direction::SOUTH);

    // Facilities
    for (auto& facilityData : room.facilities) {
        auto facility = facilities::make(coords, facilityData);
        facility->setLocalPosition(tileLocalPosition(coords) + tileSize() / 2.f);
        facility->setLocalScale(m_roomScale);
        facility->setEmitter(m_data);
        facility->centerOrigin();
        if (needWayDown && facilityData.type() == L"ladder") {
            auto ladder = dynamic_cast<facilities::Ladder*>(facility.get());
            ladder->setDesign(facilities::Ladder::Design::EXIT_MAIN);
            needWayDown = false;
        }
        tile.facilities.emplace_back(std::move(facility));
        attachChild(*tile.facilities.back());
    }

    // Add ladder if none yet
    // TODO Duplication of code...
    if (needWayDown) {
        // TODO This creates a pending reference inside ladder (which is not used, but still...)
        ElementData elementData;
        auto ladder = std::make_unique<facilities::Ladder>(coords, elementData);
        ladder->setDesign(facilities::Ladder::Design::EXIT_END);
        ladder->setLocalPosition(tileLocalPosition(coords) + tileSize() / 2.f);
        ladder->setLocalScale(m_roomScale);
        ladder->setEmitter(m_data);
        ladder->centerOrigin();
        tile.facilities.emplace_back(std::move(ladder));
        attachChild(*tile.facilities.back());
    }
}

void Inter::refreshTileTraps(const sf::Vector2u& coords)
{
    returnif (coords.x >= m_data->floorsCount());
    returnif (coords.y >= m_data->roomsByFloor());

    auto& room = m_data->room(coords);
    auto& tile = m_tiles[coords];

    // Reset
    tile.trap = nullptr;

    // Room is not constructed
    returnif (room.state != Data::RoomState::CONSTRUCTED);

    // Trap or no trap
    returnif (!room.trap.exists());

    // Trap
    tile.trap = traps::make(room);
    tile.trap->setLocalPosition(tileLocalPosition(coords) + tileSize() / 2.f);
    tile.trap->setLocalScale(m_roomScale);
    tile.trap->setEmitter(m_data);
    tile.trap->centerOrigin();
    attachChild(*tile.trap);
}

void Inter::refreshOuterWalls()
{
    const auto& roomSize = Application::context().textures.get("dungeon/inter/inner_wall").getSize();

    // West wall
    const auto& westTextureSize = Application::context().textures.get("dungeon/inter/outer_wall_west").getSize();
    sf::Vector2f westRealSize(westTextureSize.x * m_grid.cellSize().x / roomSize.x, size().y);
    sf::IntRect westWallsRect(0, 0, westTextureSize.x, westTextureSize.y * m_grid.rows());
    m_outerWalls[0].setSize(westRealSize);
    m_outerWalls[0].setPosition(-westRealSize.x, 0.f);
    m_outerWalls[0].setTextureRect(westWallsRect);

    // East wall
    const auto& eastTextureSize = Application::context().textures.get("dungeon/inter/outer_wall_east").getSize();
    sf::Vector2f eastRealSize(eastTextureSize.x * m_grid.cellSize().x / roomSize.x, size().y);
    sf::IntRect eastWallsRect(0, 0, eastTextureSize.x, eastTextureSize.y * m_grid.rows());
    m_outerWalls[1].setSize(eastRealSize);
    m_outerWalls[1].setPosition(size().x, 0.f);
    m_outerWalls[1].setTextureRect(eastWallsRect);
}
