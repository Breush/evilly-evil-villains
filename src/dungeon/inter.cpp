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
}

void Inter::init()
{
    core()->add(&m_contextMenu);
    update();
}

void Inter::update()
{
    clearParts();

    // Room tiles
    for (auto& roomTile : m_roomTiles)
    for (auto& tile : roomTile)
            addPart(&tile);

    // Grid
    addPart(&m_grid);
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
        tile.setSize(cellSize);
        tile.setPosition(m_grid.cellPosition(floorsCount - floor - 1, room));
        setRoomTileState(floor, room, floors[floor].rooms[room].state);
    }

    setStatus(true);
}

void Inter::setRoomTileState(const uint floor, const uint room, const Data::RoomState state)
{
    auto& tile = m_roomTiles[floor][room];

    if (state == Data::RoomState::VOID) tile.setFillColor(sf::Color::Transparent);
    else if (state == Data::RoomState::CONSTRUCTED) tile.setFillColor(sf::Color::Green);
    else tile.setFillColor(sf::Color::Red);
}

//------------------------//
//----- Mouse events -----//

void Inter::handleMouseEvent(const sf::Event& event, const sf::Vector2f& relPos)
{
    returnif (m_data == nullptr);

    switch (event.type) {
    case sf::Event::MouseButtonPressed:
        handleMousePressed(event, relPos);
        break;
    case sf::Event::MouseMoved:
        handleMouseMoved(event, relPos);
        break;
    case sf::Event::MouseLeft:
        handleMouseLeft();
        break;
    default:
        break;
    }
}

void Inter::handleMousePressed(const sf::Event& event, const sf::Vector2f& relPos)
{
    sf::Vector2f fixPos = getInverseTransform().transformPoint(relPos);

    if (event.mouseButton.button == sf::Mouse::Right) {
        // Context title
        std::function<void()> constructRoom = [this]() { switchSelectedRoomState(); };
        std::wstringstream roomName;
        selectRoomFromCoords(fixPos);
        roomName << _("Room") << " " << m_selectedRoom.x << " - " << m_selectedRoom.y;

        // Context construct or destroy room
        std::wstring constructRoomString;
        if (m_data->room(m_selectedRoom).state == Data::RoomState::VOID) constructRoomString = L"Construct";
        else constructRoomString = L"Destroy";

        // Context choices
        m_contextMenu.clearChoices();
        m_contextMenu.setTitle(roomName.str());
        m_contextMenu.addChoice(constructRoomString, constructRoom);

        m_contextMenu.setPosition(relPos);
        m_contextMenu.setVisible(true);
    }
    else {
        m_contextMenu.setVisible(false);
    }
}

void Inter::handleMouseMoved(const sf::Event& event, const sf::Vector2f& relPos)
{
}

void Inter::handleMouseLeft()
{
    // Not usable yet...
    // m_contextMenu.setVisible(false);
}

bool Inter::handleKeyboardEvent(const sf::Event& event)
{
#if DEBUG_GLOBAL >= 1
    // Keyboard event
    if (event.type == sf::Event::KeyPressed) {
        // Add rows
        if (event.key.code == sf::Keyboard::Add)
            m_grid.setRowsColumns(m_grid.rows() + 1, m_grid.columns());
        else if (event.key.code == sf::Keyboard::Subtract)
            m_grid.setRowsColumns(m_grid.rows() - 1, m_grid.columns());

        // Add columns
        else if (event.key.code == sf::Keyboard::Multiply)
            m_grid.setRowsColumns(m_grid.rows(), m_grid.columns() + 1);
        else if (event.key.code == sf::Keyboard::Divide)
            m_grid.setRowsColumns(m_grid.rows(), m_grid.columns() - 1);
    }
#endif

    return false;
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

void Inter::changedSize()
{
    m_grid.setSize(size());
    refreshRoomTiles();
    update();

    baseClass::changedSize();
}

