#include "dungeon/inter.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "dungeon/data.hpp"
#include "nui/uicore.hpp"
#include "tools/debug.hpp"
#include "tools/event.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <sstream>

using namespace dungeon;

Inter::Inter()
    : m_data(nullptr)
{
    setFocusable(false);
    m_contextMenu.setVisible(false);
    m_contextMenu.setZDepth(10);
}

void Inter::init()
{
    core()->add(&m_contextMenu);
    m_contextMenu.setParent(this);
    update();
}

void Inter::update()
{
    clearParts();

    // Grid
    addPart(&m_grid);

    // Room tiles
    for (auto& roomTile : m_roomTiles)
    for (auto& tile : roomTile)
            addPart(&tile);
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
    refreshRoomTiles();
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

    setStatus(true);
    update();
}

void Inter::setRoomTileState(const uint floor, const uint room, const Data::RoomState state)
{
    auto contructedRoomTexture = &Application::context().textures.get(Textures::DUNGEON_INTER_ROOM);
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
    m_contextMenu.handleGlobalEvent(event);

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

void Inter::handleMouseButtonPressed(const sf::Mouse::Button& button, const sf::Vector2f& mousePos)
{
    if (button == sf::Mouse::Right) {
        // Getting grid info
        selectRoomFromCoords(mousePos);

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
        m_contextMenu.setLocalPosition(mousePos, false);
        m_contextMenu.setOrigin({m_contextMenu.size().x / 2.f, 10.f});
        m_contextMenu.setVisible(true);
    }
}

void Inter::handleMouseMoved(const sf::Vector2f& mouseButton)
{
}

void Inter::handleMouseLeft()
{
}

bool Inter::handleKeyboardEvent(const sf::Event& event)
{
    return false;
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

sf::Vector2u& Inter::selectRoomFromCoords(const sf::Vector2f& coords)
{
    m_selectedRoom = m_grid.rowColumnFromCoords(coords);
    m_selectedRoom.x = m_data->floorsCount() - m_selectedRoom.x - 1;
    return m_selectedRoom;
}

//-------------------//
//----- Refresh -----//

void Inter::changedStatus()
{
    returnif (!status());

    m_contextMenu.setStatus(true);
}

void Inter::changedSize()
{
    m_grid.setSize(size());
    refreshRoomTiles();
    update();

    baseClass::changedSize();
}

