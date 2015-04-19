#include "dungeon/data.hpp"

#include "dungeon/graph.hpp"
#include "tools/debug.hpp"
#include "tools/string.hpp"
#include "tools/tools.hpp"

#include <pugixml/pugixml.hpp>
#include <stdexcept>

using namespace dungeon;

Data::Data()
    : m_floorsCount(0)
    , m_roomsByFloor(0)
{
}

void Data::load(const std::wstring& folder)
{
    wdebug_dungeon_1(L"Loading data from folder " << folder);

    loadDungeon(L"saves/" + folder + L"dungeon.xml");
}

void Data::save(const std::wstring& folder)
{
    #if DEBUG_GLOBAL > 0
        saveDungeon(L"saves/" + folder + L"dungeon_saved.xml");
    #else
        saveDungeon(L"saves/" + folder + L"dungeon.xml");
    #endif

    wdebug_dungeon_1(L"Saved data to folder " << folder);
}

void Data::createFiles(const std::wstring& folder)
{
    saveDungeon(L"saves/" + folder + L"dungeon.xml");

    wdebug_dungeon_1(L"Created data files to folder " << folder);
}

//-------------------//
//----- Dungeon -----//

void Data::loadDungeon(const std::wstring& file)
{
    m_floors.clear();

    // Parsing XML
    pugi::xml_document doc;
    doc.load_file(file.c_str());

    const auto& resources = doc.child(L"resources");
    const auto& dungeon = doc.child(L"dungeon");

    if (!resources || !dungeon)
        throw std::runtime_error("File " + toString(file) + " is not a valid dungeon file: resources or dungeon node not found.");

    //---- Resources

    m_dosh = resources.attribute(L"dosh").as_uint();
    m_fame = resources.attribute(L"fame").as_uint();

    //---- Dungeon

    m_name = dungeon.attribute(L"name").as_string();
    m_floorsCount = dungeon.attribute(L"floorsCount").as_uint();
    m_roomsByFloor = dungeon.attribute(L"roomsByFloor").as_uint();

    wdebug_dungeon_1(L"Dungeon is " << m_name << L" of size " << m_floorsCount << "x" << m_roomsByFloor);

    // Floors
    m_floors.reserve(m_floorsCount);
    for (auto& floor : dungeon.children(L"floor")) {
        auto floorPos = floor.attribute(L"pos").as_uint();

        m_floors.push_back({floorPos});
        mdebug_dungeon_2("Found floor " << floorPos);

        // Rooms
        m_floors[floorPos].rooms.reserve(m_roomsByFloor);
        for (auto& roomInfo : floor.children(L"room")) {
            Room room;
            room.floorPos = floorPos;
            room.pos = roomInfo.attribute(L"pos").as_uint();
            room.state = RoomState::UNKNOWN;

            std::wstring roomStateString = roomInfo.attribute(L"state").as_string();
            if (roomStateString == L"void") room.state = RoomState::VOID;
            else if (roomStateString == L"constructed") room.state = RoomState::CONSTRUCTED;

            wdebug_dungeon_3(L"Found room " << room.pos << L" of state " << roomStateString);

            // Facilities
            for (auto& roomFacilitiesInfo : roomInfo.children(L"facility")) {
                std::wstring type = roomFacilitiesInfo.attribute(L"type").as_string();

                if (type == L"ladder")  room.facilities.ladder = true;
                if (type == L"door")    room.facilities.door = true;

                wdebug_dungeon_4(L"Found facility " + type);
            }

            m_floors[floorPos].rooms.emplace_back(std::move(room));
        }
    }
}

void Data::saveDungeon(const std::wstring& file)
{
    // Creating XML
    pugi::xml_document doc;
    auto resources = doc.append_child(L"resources");
    auto dungeon = doc.append_child(L"dungeon");

    //---- Resources

    resources.append_attribute(L"dosh") = m_dosh;
    resources.append_attribute(L"fame") = m_fame;

    //---- Dungeon

    dungeon.append_attribute(L"name") = m_name.c_str();
    dungeon.append_attribute(L"floorsCount") = m_floorsCount;
    dungeon.append_attribute(L"roomsByFloor") = m_roomsByFloor;

    // Floors
    for (uint floorPos = 0; floorPos < m_floors.size(); ++floorPos) {
        mdebug_dungeon_2("Saving floor " << floorPos);
        auto floor = dungeon.append_child(L"floor");
        floor.append_attribute(L"pos") = floorPos;

        // Rooms
        for (uint roomPos = 0; roomPos < m_floors[floorPos].rooms.size(); ++roomPos) {
            auto dataRoom = m_floors[floorPos].rooms[roomPos];

            mdebug_dungeon_3("Saving room " << roomPos);
            auto room = floor.append_child(L"room");
            room.append_attribute(L"pos") = roomPos;

            RoomState roomState = dataRoom.state;
            std::wstring roomStateString = L"unknown";
            if (roomState == RoomState::VOID) roomStateString = L"void";
            else if (roomState == RoomState::CONSTRUCTED) roomStateString = L"constructed";
            room.append_attribute(L"state") = roomStateString.c_str();

            // Facilities
            if (dataRoom.facilities.door)
                room.append_child(L"facility").append_attribute(L"type") = L"door";

            if (dataRoom.facilities.ladder)
                room.append_child(L"facility").append_attribute(L"type") = L"ladder";
        }
    }

    doc.save_file(file.c_str());
}

//---------------------------//
//----- Inconsistencies -----//

void Data::correctFloorsRooms()
{
    // Floors
    m_floors.resize(m_floorsCount);
    for (uint floorPos = 0; floorPos < m_floorsCount; ++floorPos) {
        auto& floor = m_floors[floorPos];
        floor.pos = floorPos;
        // Rooms by floor
        floor.rooms.resize(m_roomsByFloor);
        for (uint roomPos = 0; roomPos < m_roomsByFloor; ++roomPos) {
            auto& ownRoom = floor.rooms[roomPos];
            ownRoom.floorPos = floorPos;
            ownRoom.pos = roomPos;
            // Unknown rooms become empty
            if (ownRoom.state == RoomState::UNKNOWN)
                ownRoom.state = RoomState::VOID;
        }
    }
}

//-----------------//
//----- Rooms -----//

bool Data::isRoomConstructed(const sf::Vector2u& roomCoord)
{
    returnif (roomCoord.x >= m_floorsCount) false;
    returnif (roomCoord.y >= m_roomsByFloor) false;
    return room(roomCoord).state != RoomState::VOID;
}

void Data::constructRoom(const sf::Vector2u& roomCoord)
{
    room(roomCoord).state = RoomState::CONSTRUCTED;

    Event event;
    event.type = EventType::ROOM_CONSTRUCTED;
    event.room = {roomCoord.x, roomCoord.y};
    EventEmitter::emit(event);

    subDosh(100u); // TODO Get value from somewhere.
}

void Data::destroyRoom(const sf::Vector2u& roomCoord)
{
    room(roomCoord).state = RoomState::VOID;

    Event event;
    event.type = EventType::ROOM_DESTROYED;
    event.room = {roomCoord.x, roomCoord.y};
    EventEmitter::emit(event);

    addDosh(85u); // TODO Get value from somewhere.
}

bool Data::roomNeighbourAccessible(const sf::Vector2u& roomCoord, Direction direction)
{
    auto neighbourCoord = roomNeighbourCoords(roomCoord, direction);
    returnif (neighbourCoord.x >= m_floorsCount) false;
    returnif (neighbourCoord.y >= m_roomsByFloor) false;

    auto& currentRoom = room(roomCoord);
    auto& neighbourRoom = room(neighbourCoord);
    returnif (neighbourRoom.state != RoomState::CONSTRUCTED) false;

    // When north or south, be sure there is a ladder
    returnif (direction == NORTH && !currentRoom.facilities.ladder) false;
    returnif (direction == SOUTH && !neighbourRoom.facilities.ladder) false;

    return true;
}

sf::Vector2u Data::roomNeighbourCoords(const sf::Vector2u& roomCoord, Direction direction)
{
    return roomCoord + roomDirectionVector(direction);
}

Data::Room& Data::roomNeighbour(const sf::Vector2u& roomCoord, Direction direction)
{
    return room(roomNeighbourCoords(roomCoord, direction));
}

sf::Vector2u Data::roomDirectionVector(Direction direction)
{
    return sf::Vector2u((direction >> 0x4) - 1u, (direction & 0xf) - 1u);
}

//---------------------//
//----- Resources -----//

void Data::addDosh(uint dosh)
{
    m_dosh += dosh;
    emit(EventType::DOSH_CHANGED);
}

void Data::subDosh(uint dosh)
{
    m_dosh -= dosh;
    emit(EventType::DOSH_CHANGED);
}

//----------------//
//----- Mode -----//

void Data::setMode(Mode mode)
{
    returnif (m_mode == mode);
    massert(m_graph != nullptr, "Graph not set before switching mode with dungeon::Data.");
    Event event;

    // Switch too design mode, no problem.
    if (mode == Mode::DESIGN) {
        m_mode = mode;
        event.type = EventType::MODE_CHANGED;
        event.mode = mode;
    }

    // For invasion, check dungeon validity.
    else if (mode == Mode::INVASION) {
        switch (m_graph->reconstructFromData()) {
        case Graph::ConstructError::NONE:
            m_mode = mode;
            event.type = EventType::MODE_CHANGED;
            event.mode = mode;
            break;

        case Graph::ConstructError::NO_DOOR:
            event.type = EventType::ERROR;
            event.message = L"No door in the dungeon. How are heroes supposed to enter?";
            break;

        case Graph::ConstructError::TOO_MANY_DOORS:
            event.type = EventType::ERROR;
            event.message = L"Too many doors. Only one door in the dungeon is supported.";
            break;
        };
    }

    EventEmitter::emit(event);
}

//-------------------------//
//----- Event emitter -----//

void Data::emit(EventType eventType)
{
    Event event;
    event.type = eventType;
    EventEmitter::emit(event);
}

//-------------------//
//----- Changes -----//

void Data::changedFloorsCount()
{
    correctFloorsRooms();
}

void Data::changedRoomsByFloor()
{
    correctFloorsRooms();
}
