#include "dungeon/inter.hpp"

#include "core/gettext.hpp"
#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "context/villains.hpp"
#include "dungeon/traps/maker.hpp"
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
    addPart(&m_grid);
}

//-------------------//
//----- Routine -----//

void Inter::onSizeChanges()
{
    returnif (size() == m_grid.size());
    m_grid.setSize(size());

    returnif (m_data == nullptr);
    refreshTiles();
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
    }
#endif
}

void Inter::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    // Selected the tile below
    selectTile(mousePos);

    // Harvest the money
    if (button == sf::Mouse::Left)
        harvestTileDosh(m_selectedTile->coords);

    // Pop the context menu up
    else if (button == sf::Mouse::Right)
        showTileContextMenu(m_selectedTile->coords, nuiPos);
}

void Inter::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    auto coords = tileFromLocalPosition(mousePos);
    setHoveredTile(coords);
}

void Inter::handleMouseLeft()
{
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
        refreshTile(coords);
        break;

    case EventType::FACILITY_CHANGED:
        coords = {event.facility.room.x, event.facility.room.y};
        refreshTileLayers(coords);
        // TODO Just needed for treasure, how can we be sure?
        refreshTileDoshLabel(coords);
        break;

    case EventType::TRAP_CHANGED:
        coords = {event.room.x, event.room.y};
        refreshTileTraps(coords);
        break;

    case EventType::HARVESTABLE_DOSH_CHANGED:
        coords = {event.room.x, event.room.y};
        refreshTileDoshLabel(coords);

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

    refreshTiles();
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

void Inter::addLayer(const sf::Vector2u& coords, TextureID textureID)
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
        setPartShader(&layer, ShaderID::NUI_HOVER);
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
        setPartShader(&layer, ShaderID::NUI_HOVER);
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
        m_contextMenu.addChoice(L"Remove " + facilityName, [this, facilityID, &coords]() {
            m_data->removeRoomFacility(coords, facilityID);
        });
    }

    // The facility is not there
    else {
        m_contextMenu.addChoice(L"Create " + facilityName, [this, facilityID, &coords]() {
            m_data->createRoomFacility(coords, facilityID);
        });
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
        m_contextMenu.addChoice(L"Construct room (-" + toWString(m_data->onCreateRoomCost) + L"d)", [this, &coords]() {
            m_data->constructRoom(coords);
        });
    }

    // Room does exists
    else {
        m_contextMenu.addChoice(L"Destroy room (+" + toWString(m_data->onDestroyRoomGain) + L"d)", [this, &coords]() {
            m_data->destroyRoom(coords);
        });

        // Facilities
        addFacilityChoice(coords, L"ladder", L"ladder");
        addFacilityChoice(coords, L"entrance", L"entrance");
    }

    // Context positions
    m_contextMenu.setLocalPosition(nuiPos);
    m_contextMenu.setOrigin({m_contextMenu.size().x / 2.f, 10.f});
    m_contextMenu.markForVisible(true);
}

//---------------------//
//----- Structure -----//

void Inter::createRoomFacility(const sf::Vector2f& relPos, const std::wstring& facilityID)
{
    m_data->createRoomFacility(tileFromLocalPosition(relPos), facilityID);
}

void Inter::setRoomTrap(const sf::Vector2f& relPos, const std::wstring& trapID)
{
    m_data->setRoomTrap(tileFromLocalPosition(relPos), trapID);
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

void Inter::refreshTiles()
{
    for (auto& tile : m_tiles)
        refreshTile(tile.second.coords);
}

void Inter::refreshTile(const sf::Vector2u& coords)
{
    refreshTileDoshLabel(coords);
    refreshTileLayers(coords);
    refreshTileTraps(coords);
}

void Inter::refreshTileDoshLabel(const sf::Vector2u& coords)
{
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
    const auto& room = m_data->room(coords);
    const auto state = room.state;

    // Reset
    clearLayers(coords);

    // Room is not constructed
    returnif (state == Data::RoomState::VOID);

    // Room is somehow bugged
    if (state == Data::RoomState::UNKNOWN) {
        std::cerr << "/!\\ Found a room with unknown state at " << coords << "." << std::endl;
        std::cerr << "If that is a reccurent issue, please report this bug." << std::endl;
        addLayer(coords, TextureID::DEFAULT);
        return;
    }

    // Add room textures
    addLayer(coords, TextureID::DUNGEON_INTER_ROOM);

    // Facilities
    // TODO Use same structure than Traps!! facilities::make()
    for (const auto& facility : room.facilities) {
        if (facility.type() == L"ladder")           addLayer(coords, TextureID::DUNGEON_INTER_LADDER);
        else if (facility.type() == L"treasure")    addLayer(coords, TextureID::DUNGEON_INTER_TREASURE);
        else if (facility.type() == L"entrance")    addLayer(coords, TextureID::DUNGEON_INTER_ENTRANCE);
        else std::wcout << L"Unreferenced facility '" << facility.type() << "' texture, ignoring it." << std::endl;
    }
}

void Inter::refreshTileTraps(const sf::Vector2u& coords)
{
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
    tile.trap->centerOrigin();
    tile.trap->setEmitter(m_data);
    attachChild(*tile.trap);
}
