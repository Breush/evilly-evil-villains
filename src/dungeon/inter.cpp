#include "dungeon/inter.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "core/gettext.hpp"
#include "tools/debug.hpp"
#include "tools/event.hpp"
#include "tools/tools.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <sstream>

using namespace dungeon;

Inter::Inter(nui::ContextMenu& contextMenu)
    : m_contextMenu(contextMenu)
{
    update();
}

//-------------------//
//----- Routine -----//

void Inter::update()
{
    returnif (m_data == nullptr);

    m_grid.setSize(size());
    refreshRoomTiles();

    clearParts();

    // Grid
    addPart(&m_grid);

    // Room tiles
    for (auto& roomTile : m_roomTiles)
    for (auto& tile : roomTile)
        addPart(&tile);

    if (m_hasRoomSelected)
        setPartShader(&m_roomTiles[m_selectedRoom.x][m_selectedRoom.y], ShaderID::NUI_HOVER);
}

//--------------------------//
//----- Dungeon events -----//

void Inter::receive(const Event& event)
{
    if (event.type == EventType::ROOM_DESTROYED
        || event.type == EventType::ROOM_CONSTRUCTED)
        refreshRoomTiles();
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
    m_roomTiles.clear();

    const auto& roomsByFloor = m_data->roomsByFloor();
    const auto& floorsCount = m_data->floorsCount();

    // Grid
    m_grid.setRowsColumns(floorsCount, roomsByFloor);

    // Room tiles
    m_roomTiles.resize(floorsCount);
    for (auto& roomTile : m_roomTiles)
        roomTile.resize(roomsByFloor);

    m_hasRoomSelected = false;
    update();
}

//---------------------------//
//----- Room management -----//

void Inter::refreshRoomTiles()
{
    const auto& roomsByFloor = m_data->roomsByFloor();
    const auto& floorsCount = m_data->floorsCount();
    const auto& cellSize = m_grid.cellSize();

    for (uint floor = 0; floor < floorsCount; ++floor)
    for (uint room = 0; room < roomsByFloor; ++room) {
        auto& tile = m_roomTiles[floor][room];
        tile.setSize({1.3f * cellSize.x, 1.1f * cellSize.y}); // FIXME Use separate images to get this effect
        tile.setPosition(roomLocalPosition({floor, room}));
        setRoomTile(floor, room, m_data->room({floor, room}));
    }
}

void Inter::setRoomTile(const uint floor, const uint room, const Data::Room& roomInfo)
{
    const auto state = roomInfo.state;
    auto ladderRoomTexture =    &Application::context().textures.get(TextureID::DUNGEON_INTER_LADDER_ROOM);
    auto doorRoomTexture =      &Application::context().textures.get(TextureID::DUNGEON_INTER_DOOR_ROOM);
    auto roomTexture =          &Application::context().textures.get(TextureID::DUNGEON_INTER_ROOM);
    auto& tile = m_roomTiles[floor][room];

    // Reset
    tile.setTexture(nullptr);
    tile.setFillColor(sf::Color::White);

    // Selecting
    switch (state)
    {
        case Data::RoomState::VOID:
            tile.setFillColor(sf::Color::Transparent);
            break;
        case Data::RoomState::CONSTRUCTED:
            if (roomInfo.facilities.ladder)     tile.setTexture(ladderRoomTexture);
            else if (roomInfo.facilities.door)  tile.setTexture(doorRoomTexture);
            else                                tile.setTexture(roomTexture);
            break;
        default:
            tile.setFillColor(sf::Color::Red);
            break;
    }
}

sf::Vector2f Inter::roomSize() const
{
    return m_grid.cellSize();
}

sf::Vector2f Inter::roomLocalPosition(const sf::Vector2u& room) const
{
    return m_grid.cellPosition(m_data->floorsCount() - 1u - room.x, room.y);
}

//------------------------//
//----- Mouse events -----//

void Inter::handleGlobalEvent(const sf::Event& event)
{
    // Remove selection every time
    if (event.type == sf::Event::MouseButtonPressed)
        setHasRoomSelected(false);

#if DEBUG_GLOBAL > 0
    // Keyboard event
    if (event.type == sf::Event::KeyPressed) {
        // Add rows
        if (event.key.code == sf::Keyboard::Add)
            adaptFloorsCount(1);
        else if (event.key.code == sf::Keyboard::Subtract)
            adaptFloorsCount(-1);

        // Add columns
        else if (event.key.code == sf::Keyboard::Multiply)
            adaptRoomsByFloor(1);
        else if (event.key.code == sf::Keyboard::Divide)
            adaptRoomsByFloor(-1);
    }
#endif
}

void Inter::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    // Selected new room
    selectRoomFromCoords(mousePos);

    // Pop the context menu up
    if (button == sf::Mouse::Right)
        showRoomContextMenu(m_selectedRoom, nuiPos);
}

void Inter::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    refreshRoomSelectedShader();

    auto room = roomFromCoords(mousePos);
    setPartShader(&m_roomTiles[room.x][room.y], ShaderID::NUI_HOVER);
}

void Inter::handleMouseLeft()
{
    refreshRoomSelectedShader();
}

//------------------------//
//----- Context menu -----//

void Inter::showRoomContextMenu(const sf::Vector2u& room, const sf::Vector2f& nuiPos)
{
    m_contextMenu.clearChoices();

    // Context title
    std::wstringstream roomName;
    roomName << _("Room") << " " << room.x << "/" << room.y;
    m_contextMenu.setTitle(roomName.str());

    // Room does not exists yet
    if (m_data->room(m_selectedRoom).state == Data::RoomState::VOID) {

        m_contextMenu.addChoice(L"Construct room (-100d)", [this]() {
            constructRoom(m_selectedRoom);
        });

    }
    // Room does exists
    else {

        m_contextMenu.addChoice(L"Destroy room (+85d)", [this]() {
            destroyRoom(m_selectedRoom);
        });

    }

    // Context positions
    m_contextMenu.setLocalPosition(nuiPos);
    m_contextMenu.setOrigin({m_contextMenu.size().x / 2.f, 10.f});
    m_contextMenu.markForVisible(true);
}

//----------------------------------//
//----- Direct data management -----//

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

//-----------------------//
//----- Interaction -----//

void Inter::constructRoom(const sf::Vector2u& room)
{
    assert(!m_data->isRoomConstructed(room));
    m_data->constructRoom(room);
}

void Inter::destroyRoom(const sf::Vector2u& room)
{
    assert(m_data->isRoomConstructed(room));
    m_data->destroyRoom(room);
}

sf::Vector2u Inter::roomFromCoords(const sf::Vector2f& coords)
{
    auto room = m_grid.rowColumnFromCoords(coords);
    room.x = m_data->floorsCount() - room.x - 1;
    return room;
}

void Inter::selectRoomFromCoords(const sf::Vector2f& coords)
{
    m_selectedRoom = roomFromCoords(coords);
    setHasRoomSelected(true);
}

//----------------------//
//----- Facilities -----//

void Inter::constructDoor(const sf::Vector2f& relPos)
{
    auto room = roomFromCoords(relPos);

    if (m_data->room(room).state == Data::RoomState::CONSTRUCTED) {
        if (!m_data->room(room).facilities.door)
            m_data->room(room).facilities.door = true;
    }

    // FIXME Limit to only one door in the dungeon

    // TODO Don't really need to refresh texture on all tiles
    refreshRoomTiles();
}

void Inter::constructLadder(const sf::Vector2f& relPos)
{
    auto room = roomFromCoords(relPos);

    if (m_data->room(room).state == Data::RoomState::CONSTRUCTED) {
        if (!m_data->room(room).facilities.ladder)
            m_data->room(room).facilities.ladder = true;
    }

    // TODO Don't really need to refresh texture on all tiles
    refreshRoomTiles();
}

//-------------------//
//----- Display -----//

void Inter::setHasRoomSelected(bool hasRoomSelected)
{
    m_hasRoomSelected = hasRoomSelected;
    refreshRoomSelectedShader();
}

void Inter::refreshRoomSelectedShader()
{
    resetPartsShader();

    if (m_hasRoomSelected)
        setPartShader(&m_roomTiles[m_selectedRoom.x][m_selectedRoom.y], ShaderID::NUI_HOVER);
}
