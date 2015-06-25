#include "dungeon/data.hpp"

#include "dungeon/graph.hpp"
#include "dungeon/hero.hpp"
#include "dungeon/traps.hpp"
#include "context/villains.hpp"
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

//---------------------------//
//----- File management -----//

std::wstring Data::load(const std::wstring& folder)
{
    wdebug_dungeon_1(L"Loading data from folder " << folder);

    context::villains.load();
    m_villain = context::villains.getFromWorldFolder(folder);
    m_villain->doshWallet.setEvents(this, EventType::DOSH_CHANGED);

    std::wstring mainDungeonFilename = L"saves/" + folder + L"dungeon.xml";
    loadDungeon(mainDungeonFilename);
    return mainDungeonFilename;
}

std::wstring Data::save(const std::wstring& folder)
{
    #if DEBUG_GLOBAL > 0
        std::wstring mainDungeonFilename = L"saves/" + folder + L"dungeon_saved.xml";
    #else
        std::wstring mainDungeonFilename = L"saves/" + folder + L"dungeon.xml";
    #endif

    saveDungeon(mainDungeonFilename);

    wdebug_dungeon_1(L"Saved data to folder " << folder);

    context::villains.save();

    return mainDungeonFilename;
}

void Data::createFiles(const std::wstring& folder)
{
    saveDungeon(L"saves/" + folder + L"dungeon.xml");

    wdebug_dungeon_1(L"Created data files to folder " << folder);
}

//--------------------------//
//----- XML management -----//

void Data::loadDungeon(const std::wstring& file)
{
    m_floors.clear();

    // Parsing XML
    pugi::xml_document doc;
    doc.load_file(file.c_str());

    const auto& dungeon = doc.child(L"dungeon");

    if (!dungeon)
        throw std::runtime_error("File " + toString(file) + " is not a valid dungeon file: resources or dungeon node not found.");

    //---- Dungeon

    m_name = dungeon.attribute(L"name").as_string();
    m_floorsCount = dungeon.attribute(L"floorsCount").as_uint();
    m_roomsByFloor = dungeon.attribute(L"roomsByFloor").as_uint();
    m_fame = dungeon.attribute(L"fame").as_uint();

    wdebug_dungeon_1(L"Dungeon is " << m_name << L" of size " << m_floorsCount << "x" << m_roomsByFloor);

    // Floors
    m_floors.reserve(m_floorsCount);
    for (auto& floor : dungeon.children(L"floor")) {
        auto floorPos = floor.attribute(L"pos").as_uint();

        m_floors.push_back({floorPos});
        mdebug_dungeon_2("Found floor " << floorPos);

        // Rooms
        m_floors[floorPos].rooms.reserve(m_roomsByFloor);
        for (auto& roomNode : floor.children(L"room")) {
            Room room;
            room.coords.x = floorPos;
            room.coords.y = roomNode.attribute(L"pos").as_uint();

            std::wstring roomStateString = roomNode.attribute(L"state").as_string();
            if (roomStateString == L"void") room.state = RoomState::VOID;
            else if (roomStateString == L"constructed") room.state = RoomState::CONSTRUCTED;

            wdebug_dungeon_3(L"Found room " << room.pos << L" of state " << roomStateString);

            // Elements
            room.trap.loadXML(roomNode);
            loadDungeonRoomFacilities(room, roomNode);

            // Add the room
            m_floors[floorPos].rooms.emplace_back(std::move(room));
        }
    }
}

void Data::loadDungeonRoomFacilities(Room& room, const pugi::xml_node& node)
{
    // Reset
    for (auto& facility : room.facilities)
        facility = false;

    // TODO Same as trap: have an easy adder for flexibility and modding API
    for (const auto& facilities : node.children(L"facility")) {
        std::wstring type = facilities.attribute(L"type").as_string();

        if (type == L"ladder")
            room.facilities[FacilityID::LADDER] = true;
        else if (type == L"entrance")
            room.facilities[FacilityID::ENTRANCE] = true;
        else if (type == L"treasure") {
            room.facilities[FacilityID::TREASURE] = true;
            room.treasureDosh = facilities.attribute(L"dosh").as_uint();
        }
    }
}

void Data::saveDungeon(const std::wstring& file)
{
    // Creating XML
    pugi::xml_document doc;
    auto dungeon = doc.append_child(L"dungeon");

    //---- Dungeon

    dungeon.append_attribute(L"name") = m_name.c_str();
    dungeon.append_attribute(L"floorsCount") = m_floorsCount;
    dungeon.append_attribute(L"roomsByFloor") = m_roomsByFloor;
    dungeon.append_attribute(L"fame") = m_fame;

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

            // Elements
            dataRoom.trap.saveXML(room);
            saveDungeonRoomFacilities(dataRoom, room);
        }
    }

    doc.save_file(file.c_str());
}

void Data::saveDungeonRoomFacilities(const Room& room, pugi::xml_node& node)
{
    // TODO Use same structure than TrapData
    if (room.facilities[FacilityID::LADDER])
        node.append_child(L"facility").append_attribute(L"type") = L"ladder";

    if (room.facilities[FacilityID::ENTRANCE])
        node.append_child(L"facility").append_attribute(L"type") = L"entrance";

    if (room.facilities[FacilityID::TREASURE]) {
        auto facility = node.append_child(L"facility");
        facility.append_attribute(L"type") = L"treasure";
        facility.append_attribute(L"dosh") = room.treasureDosh;
    }
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
            ownRoom.coords.x = floorPos;
            ownRoom.coords.y = roomPos;

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
    returnif (!m_villain->doshWallet.sub(onCreateRoomCost));
    room(roomCoord).state = RoomState::CONSTRUCTED;

    Event event;
    event.type = EventType::ROOM_CONSTRUCTED;
    event.room = {roomCoord.x, roomCoord.y};
    EventEmitter::emit(event);
}

void Data::destroyRoom(const sf::Vector2u& roomCoord)
{
    m_villain->doshWallet.add(onDestroyRoomGain);
    room(roomCoord).state = RoomState::VOID;

    // TODO Also destroyed all facilities inside.

    Event event;
    event.type = EventType::ROOM_DESTROYED;
    event.room = {roomCoord.x, roomCoord.y};
    EventEmitter::emit(event);
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
    returnif (direction == NORTH && !currentRoom.facilities[FacilityID::LADDER]) false;
    returnif (direction == SOUTH && !neighbourRoom.facilities[FacilityID::LADDER]) false;

    return true;
}

sf::Vector2u Data::roomNeighbourCoords(const sf::Vector2u& coords, Direction direction)
{
    return coords + roomDirectionVector(direction);
}

Data::Room& Data::roomNeighbour(const sf::Vector2u& coords, Direction direction)
{
    return room(roomNeighbourCoords(coords, direction));
}

sf::Vector2u Data::roomDirectionVector(Direction direction)
{
    return sf::Vector2u((direction >> 0x4) - 1u, (direction & 0xf) - 1u);
}

//---------------------//
//----- Treasures -----//

void Data::stealTreasure(const sf::Vector2u& coords, Hero& hero, uint stolenDosh)
{
    auto& roomInfo = room(coords);
    assert(stolenDosh <= roomInfo.treasureDosh);

    roomInfo.treasureDosh -= stolenDosh;
    hero.addDosh(stolenDosh);

    Event event;
    event.type = EventType::FACILITY_CHANGED;
    event.facility.id = FacilityID::TREASURE;
    event.facility.room = {coords.x, coords.y};
    EventEmitter::emit(event);
}

//--------------------------------//
//----- Facilities and traps -----//

void Data::setRoomFacility(const sf::Vector2u& coords, FacilityID facilityID, bool state)
{
    auto& roomInfo = room(coords);
    returnif (roomInfo.facilities[facilityID] == state);

    roomInfo.facilities[facilityID] = state;

    Event event;
    event.type = EventType::FACILITY_CHANGED;
    event.facility.id = facilityID;
    event.facility.room = {coords.x, coords.y};
    EventEmitter::emit(event);

    // TODO Dispatch this management to the facility itself
    if (facilityID == FacilityID::TREASURE) {
        // Takes 100 dosh, or 0u if impossible
        if (m_villain->doshWallet.sub(100u))
            roomInfo.treasureDosh = 100u;
    }
}

void Data::setRoomTrap(const sf::Vector2u& coords, const std::wstring& trapID)
{
    auto& roomInfo = room(coords);
    returnif (roomInfo.trap.exists() && roomInfo.trap.type() == trapID);

    // Continue if and only if wallet authorize us
    if (m_villain->doshWallet.addsub(roomInfo.trap.onDestroyGain(), roomInfo.trap.onCreateCost())) {
        // Destroy previous trap if any
        roomInfo.trap.clear();

        // Set the trap to the new one.
        roomInfo.trap.create(trapID);

        // Emit event
        Event event;
        event.type = EventType::TRAP_CHANGED;
        event.room = {coords.x, coords.y};
        EventEmitter::emit(event);
    }
}

//---------------------//
//----- Resources -----//

void Data::setFame(uint value)
{
    m_fame = value;
    emit(EventType::FAME_CHANGED);
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
