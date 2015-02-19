#include "dungeon/inter.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "core/gettext.hpp"
#include "dungeon/data.hpp"
#include "tools/debug.hpp"
#include "tools/event.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <sstream>

using namespace dungeon;

Inter::Inter(nui::ContextMenu& contextMenu)
    : m_data(nullptr)
    , m_contextMenu(contextMenu)
    , m_hasRoomSelected(false)
{
    update();
}

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

//------------------------//
//----- Dungeon data -----//

void Inter::useData(dungeon::Data& data)
{
    m_data = &data;
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
    const auto& floors = m_data->floors();

    for (uint floor = 0; floor < floorsCount; ++floor)
    for (uint room = 0; room < roomsByFloor; ++room) {
        auto& tile = m_roomTiles[floor][room];
        tile.setSize({1.3f * cellSize.x, 1.1f * cellSize.y}); // FIXME Use separate images to get this effect
        tile.setPosition(m_grid.cellPosition(floorsCount - floor - 1, room));
        setRoomTileState(floor, room, floors[floor].rooms[room].state);
    }
}

void Inter::setRoomTileState(const uint floor, const uint room, const Data::RoomState state)
{
    auto contructedRoomTexture = &Application::context().textures.get(TextureID::DUNGEON_INTER_ROOM);
    auto& tile = m_roomTiles[floor][room];

    // Reset
    tile.setTexture(nullptr);
    tile.setFillColor(sf::Color::White);

    // Selecting
    // TODO See if we can use some prototype design pattern
    if (state == Data::RoomState::VOID)
        tile.setFillColor(sf::Color::Transparent);
    else if (state == Data::RoomState::CONSTRUCTED)
        tile.setTexture(contructedRoomTexture);
    else
        tile.setFillColor(sf::Color::Red);
}

//------------------------//
//----- Mouse events -----//

void Inter::handleGlobalEvent(const sf::Event& event)
{
    // Remove selection every time
    if (event.type == sf::Event::MouseButtonPressed)
        setHasRoomSelected(false);

#if DEBUG_GLOBAL >= 1
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

void Inter::handleMouseButtonPressed(const sf::Mouse::Button& button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    // Selected new room
    selectRoomFromCoords(mousePos);

    // Pop the context menu up
    if (button == sf::Mouse::Right) {
        // Context title
        std::wstringstream roomName;
        roomName << _("Room") << " " << m_selectedRoom.x << "/" << m_selectedRoom.y;

        // Context construct or destroy room
        std::wstring constructRoomString;
        if (m_data->room(m_selectedRoom).state == Data::RoomState::VOID) constructRoomString = L"Construct";
        else constructRoomString = L"Destroy";
        std::function<void()> constructRoom = [this]() { switchSelectedRoomState(); };

        // Context choices
        m_contextMenu.clearChoices();
        m_contextMenu.setTitle(roomName.str());
        m_contextMenu.addChoice(constructRoomString, constructRoom);

        // Context positions
        m_contextMenu.setLocalPosition(nuiPos);
        m_contextMenu.setOrigin({m_contextMenu.size().x / 2.f, 10.f});
        m_contextMenu.markForVisible(true);
    }
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

void Inter::switchSelectedRoomState()
{
    if (m_data->room(m_selectedRoom).state == Data::RoomState::VOID)
        m_data->room(m_selectedRoom).state = Data::RoomState::CONSTRUCTED;
    else if (m_data->room(m_selectedRoom).state == Data::RoomState::CONSTRUCTED)
        m_data->room(m_selectedRoom).state = Data::RoomState::VOID;

    refreshRoomTiles();
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
