#include "dungeon/data.hpp"

#include "dungeon/debug.hpp"
#include "dungeon/graph.hpp"
#include "dungeon/hero.hpp"
#include "dungeon/traps.hpp"
#include "dungeon/facilities/maker.hpp"
#include "context/villains.hpp"
#include "tools/string.hpp"
#include "tools/tools.hpp"
#include "tools/filesystem.hpp"

#include <pugixml/pugixml.hpp>
#include <stdexcept>

using namespace dungeon;

Data::Data()
    : m_floorsCount(0u)
    , m_roomsByFloor(0u)
    , m_timeGameHour(1.f)
{
    m_fameWallet.setEvents(this, "fame_changed");
}

//-------------------//
//----- Routine -----//

void Data::update(const sf::Time& dt)
{
    m_timeBuffer += dt.asSeconds();

    while (m_timeBuffer >= m_timeGameHour) {
        m_timeBuffer -= m_timeGameHour;
        m_time += 1u;

        emit("time_changed");
    }
}

//---------------------------//
//----- File management -----//

std::wstring Data::load(const std::wstring& folder)
{
    wdebug_dungeon_1(L"Loading data from folder " << folder);

    context::villains.load();
    m_villain = context::villains.getFromWorldFolder(folder);

    // We keep that case possible for tests.
    if (m_villain == nullptr) {
        std::wcout << L"/!\\ No villain found for folder '" << folder << "'." << std::endl;
        std::wcout << L" |  If the game crashes soon, that could be related." << std::endl;
        std::wcout << L" |  If you are running a test file, no problem here." << std::endl;
    }
    else {
        m_villain->doshWallet.setEvents(this, "dosh_changed");
    }

    #if DEBUG_GLOBAL > 0
        std::wstring mainDungeonFilename = L"saves/" + folder + L"dungeon_saved.xml";
        if (!fileExists(mainDungeonFilename))
            mainDungeonFilename = L"saves/" + folder + L"dungeon.xml";
    #else
        std::wstring mainDungeonFilename = L"saves/" + folder + L"dungeon.xml";
    #endif

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
    m_timeBuffer = 0.f;
    m_floors.clear();
    m_monstersInfo.active.clear();
    m_monstersInfo.reserve.clear();

    // Parsing XML
    pugi::xml_document doc;
    doc.load_file(file.c_str());

    const auto& dungeon = doc.child(L"dungeon");

    if (!dungeon)
        throw std::runtime_error("File " + toString(file) + " is not a valid dungeon file.");

    //---- Dungeon

    m_name = dungeon.attribute(L"name").as_string();
    m_time = dungeon.attribute(L"time").as_uint();
    m_floorsCount = dungeon.attribute(L"floorsCount").as_uint();
    m_roomsByFloor = dungeon.attribute(L"roomsByFloor").as_uint();
    setFame(dungeon.attribute(L"fame").as_uint());
    wdebug_dungeon_1(L"Dungeon is " << m_name << L" of size " << m_floorsCount << L"x" << m_roomsByFloor << L".");

    //---- Monsters

    // Reserve
    const auto& monstersNode = dungeon.child(L"monsters");
    const auto& reserveNode = monstersNode.child(L"reserve");
    for (const auto& monsterCageNode : reserveNode.children(L"monsterCage")) {
        MonsterCageInfo monsterCageInfo;
        monsterCageInfo.type = monsterCageNode.attribute(L"type").as_string();
        monsterCageInfo.countdown = monsterCageNode.attribute(L"countdown").as_uint();
        for (const auto& monsterNode : monsterCageNode.children(L"monster")) {
            MonsterInfo monsterInfo;
            monsterInfo.data.loadXML(monsterNode);
            monsterCageInfo.monsters.emplace_back(std::move(monsterInfo));
        }
        m_monstersInfo.reserve.emplace_back(std::move(monsterCageInfo));
    }

    // Active
    const auto& activeNode = monstersNode.child(L"active");
    for (const auto& monsterNode : activeNode.children(L"monster")) {
        MonsterInfo monsterInfo;
        monsterInfo.data.loadXML(monsterNode);
        m_monstersInfo.active.emplace_back(std::move(monsterInfo));
    }

    //---- Structure

    // Floors
    m_floors.reserve(m_floorsCount);
    for (const auto& floor : dungeon.children(L"floor")) {
        auto floorPos = floor.attribute(L"pos").as_uint();

        m_floors.push_back({floorPos});
        mdebug_dungeon_2("Found floor " << floorPos);

        // Rooms
        m_floors[floorPos].rooms.reserve(m_roomsByFloor);
        for (const auto& roomNode : floor.children(L"room")) {
            Room room;
            room.coords.x = floorPos;
            room.coords.y = roomNode.attribute(L"pos").as_uint();

            std::wstring roomStateString = roomNode.attribute(L"state").as_string();
            if (roomStateString == L"void") room.state = RoomState::VOID;
            else if (roomStateString == L"constructed") room.state = RoomState::CONSTRUCTED;

            wdebug_dungeon_3(L"Found room " << room.pos << L" of state " << roomStateString);

            // Elements
            const auto& trapNode = roomNode.child(L"trap");
            if (trapNode) room.trap.loadXML(trapNode);
            for (const auto& facilityNode : roomNode.children(L"facility")) {
                room.facilities.emplace_back();
                room.facilities.back().loadXML(facilityNode);
            }

            // Add the room
            m_floors[floorPos].rooms.emplace_back(std::move(room));
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
    dungeon.append_attribute(L"time") = m_time;
    dungeon.append_attribute(L"floorsCount") = m_floorsCount;
    dungeon.append_attribute(L"roomsByFloor") = m_roomsByFloor;
    dungeon.append_attribute(L"fame") = fame();

    //---- Monsters

    // Reserve
    auto monstersNode = dungeon.append_child(L"monsters");
    auto reserveNode = monstersNode.append_child(L"reserve");
    for (const auto& monsterCage : m_monstersInfo.reserve) {
        auto monsterCageNode = reserveNode.append_child(L"monsterCage");
        monsterCageNode.append_attribute(L"type") = monsterCage.type.c_str();
        monsterCageNode.append_attribute(L"countdown") = monsterCage.countdown;
        for (const auto& monsterInfo : monsterCage.monsters) {
            auto monsterNode = monsterCageNode.append_child(L"monster");
            monsterInfo.data.saveXML(monsterNode);
        }
    }

    // Active
    auto activeNode = monstersNode.append_child(L"active");
    for (const auto& monsterInfo : m_monstersInfo.active) {
        auto monsterNode = activeNode.append_child(L"monster");
        monsterInfo.data.saveXML(monsterNode);
    }

    //---- Structure

    // Floors
    for (uint floorPos = 0; floorPos < m_floors.size(); ++floorPos) {
        mdebug_dungeon_2("Saving floor " << floorPos);
        auto floor = dungeon.append_child(L"floor");
        floor.append_attribute(L"pos") = floorPos;

        // Rooms
        for (uint roomPos = 0; roomPos < m_floors[floorPos].rooms.size(); ++roomPos) {
            auto room = m_floors[floorPos].rooms[roomPos];

            mdebug_dungeon_3("Saving room " << roomPos);
            auto roomNode = floor.append_child(L"room");
            roomNode.append_attribute(L"pos") = roomPos;

            RoomState roomState = room.state;
            std::wstring roomStateString = L"unknown";
            if (roomState == RoomState::VOID) roomStateString = L"void";
            else if (roomState == RoomState::CONSTRUCTED) roomStateString = L"constructed";
            roomNode.append_attribute(L"state") = roomStateString.c_str();

            // Trap
            if (room.trap.exists()) {
                auto trapNode = roomNode.append_child(L"trap");
                room.trap.saveXML(trapNode);
            }

            // Facilities
            for (const auto& facility : room.facilities) {
                auto facilityNode = roomNode.append_child(L"facility");
                facility.saveXML(facilityNode);
            }
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

bool Data::isRoomConstructed(const sf::Vector2u& coords)
{
    returnif (coords.x >= m_floorsCount) false;
    returnif (coords.y >= m_roomsByFloor) false;
    return room(coords).state != RoomState::VOID;
}

void Data::constructRoom(const sf::Vector2u& coords, bool hard)
{
    returnif (coords.x >= m_floorsCount);
    returnif (coords.y >= m_roomsByFloor);
    returnif (room(coords).state != RoomState::VOID);

    returnif (!hard && !m_villain->doshWallet.sub(onConstructRoomCost));
    room(coords).state = RoomState::CONSTRUCTED;

    Event event;
    event.type = "room_constructed";
    event.room = {coords.x, coords.y};
    EventEmitter::emit(event);
}

void Data::destroyRoom(const sf::Vector2u& coords, bool hard)
{
    returnif (coords.x >= m_floorsCount);
    returnif (coords.y >= m_roomsByFloor);
    returnif (room(coords).state == RoomState::VOID);

    // TODO Hard should not retrieve money from facilities/traps.

    m_villain->doshWallet.add(onDestroyRoomGain);
    room(coords).state = RoomState::VOID;

    // Clear elements
    removeRoomFacilities(coords);
    removeRoomTrap(coords);

    // Destroy monsters inside
    std::erase_if(m_monstersInfo.active, [coords] (const MonsterInfo& monsterInfo) {
        sf::Vector2u monsterCoords;
        monsterCoords.x = static_cast<uint>(monsterInfo.data.at(L"rx").as_float());
        monsterCoords.y = static_cast<uint>(monsterInfo.data.at(L"ry").as_float());
        return monsterCoords == coords;
    });

    Event event;
    event.type = "room_destroyed";
    event.room = {coords.x, coords.y};
    EventEmitter::emit(event);
}

bool Data::roomNeighbourAccessible(const sf::Vector2u& coords, Direction direction)
{
    auto neighbourCoord = roomNeighbourCoords(coords, direction);
    returnif (neighbourCoord.x >= m_floorsCount) false;
    returnif (neighbourCoord.y >= m_roomsByFloor) false;

    auto& currentRoom = room(coords);
    auto& neighbourRoom = room(neighbourCoord);
    returnif (neighbourRoom.state != RoomState::CONSTRUCTED) false;

    // When north or south, be sure there is a ladder
    returnif (direction == NORTH && !hasOfType(currentRoom.facilities, L"ladder")) false;
    returnif (direction == SOUTH && !hasOfType(neighbourRoom.facilities, L"ladder")) false;

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

uint Data::roomTreasureDosh(const sf::Vector2u& coords)
{
    auto& roomInfo = room(coords);
    auto treasureFacilityIt = findOfType(roomInfo.facilities, L"treasure");
    returnif (treasureFacilityIt == std::end(roomInfo.facilities)) 0u;
    return (*treasureFacilityIt)[L"dosh"].as_uint32();
}

//---------------------//
//----- Treasures -----//

void Data::stealTreasure(const sf::Vector2u& coords, Hero& hero, uint stolenDosh)
{
    auto& roomInfo = room(coords);
    auto treasureFacilityIt = findOfType(roomInfo.facilities, L"treasure");
    assert(treasureFacilityIt != std::end(roomInfo.facilities));

    auto& treasureFacility = *treasureFacilityIt;
    assert(stolenDosh <= treasureFacility[L"dosh"].as_uint32());

    treasureFacility[L"dosh"].as_uint32() -= stolenDosh;
    hero.addDosh(stolenDosh);

    Event event;
    event.type = "facility_changed";
    event.facility.room = {coords.x, coords.y};
    EventEmitter::emit(event);
}

//--------------------------------//
//----- Facilities and traps -----//

void Data::createRoomFacility(const sf::Vector2u& coords, const std::wstring& facilityID)
{
    auto& roomInfo = room(coords);
    returnif (hasOfType(roomInfo.facilities, facilityID));

    returnif (!m_villain->doshWallet.sub(facilities::onCreateCost(facilityID)));

    roomInfo.facilities.emplace_back();
    auto& facility = roomInfo.facilities.back();
    facility.create(facilityID);

    Event event;
    event.type = "facility_changed";
    event.facility.room = {coords.x, coords.y};
    EventEmitter::emit(event);
}

void Data::removeRoomFacility(const sf::Vector2u& coords, const std::wstring& facilityID)
{
    auto& roomInfo = room(coords);
    auto found = std::find_if(roomInfo.facilities, [&](const ElementData& data) { return data.type() == facilityID; });
    wassert(found != std::end(roomInfo.facilities), L"Cannot find (and remove) facility '" << facilityID << L"' in room " << coords);

    auto& facility = *found;
    m_villain->doshWallet.add(facilities::onDestroyGain(facility));
    roomInfo.facilities.erase(found);

    Event event;
    event.type = "facility_changed";
    event.facility.room = {coords.x, coords.y};
    EventEmitter::emit(event);
}

void Data::removeRoomFacilities(const sf::Vector2u& coords)
{
    auto& roomInfo = room(coords);
    returnif (roomInfo.facilities.empty());

    for (const auto& facility : roomInfo.facilities)
        m_villain->doshWallet.add(facilities::onDestroyGain(facility));
    roomInfo.facilities.clear();

    Event event;
    event.type = "facility_changed";
    event.facility.room = {coords.x, coords.y};
    EventEmitter::emit(event);
}

void Data::setRoomTrap(const sf::Vector2u& coords, const std::wstring& trapID)
{
    // TODO Should the test be done in Inter and data just really sets the trap?
    // -> Surely yes...

    auto& roomInfo = room(coords);
    returnif (roomInfo.trap.exists() && roomInfo.trap.type() == trapID);

    // Continue if and only if wallet authorize us
    // TODO Use trapsDB data (in Inter!)
    // returnif (!m_villain->doshWallet.addsub(traps::onDestroyGain(roomInfo.trap), traps::onCreateCost(trapID)));

    // Destroy previous trap if any
    roomInfo.trap.clear();

    // Set the trap to the new one.
    roomInfo.trap.create(trapID);

    // Emit event
    Event event;
    event.type = "trap_changed";
    event.room = {coords.x, coords.y};
    EventEmitter::emit(event);
}

void Data::removeRoomTrap(const sf::Vector2u& coords)
{
    auto& roomInfo = room(coords);
    returnif (!roomInfo.trap.exists());

    // TODO Use trapsDB data (in Inter!)
    // m_villain->doshWallet.add(traps::onDestroyGain(roomInfo.trap));
    roomInfo.trap.clear();

    // Emit event
    Event event;
    event.type = "trap_changed";
    event.room = {coords.x, coords.y};
    EventEmitter::emit(event);
}

bool Data::addMonsterValid(const sf::Vector2u& coords, const std::wstring& monsterID)
{
    // Does room exist?
    auto& roomInfo = room(coords);
    returnif (roomInfo.state != RoomState::CONSTRUCTED) false;

    return true;
}

void Data::addMonsterToReserve(const std::wstring& monsterID)
{
    auto& reserve = m_monstersInfo.reserve;
    auto pMonsterCage = std::find_if(reserve, [&monsterID] (const MonsterCageInfo& monsterCage) { return monsterCage.type == monsterID; });

    // No previous cage, create it
    if (pMonsterCage == std::end(reserve)) {
        m_monstersInfo.reserve.emplace_back();
        pMonsterCage = std::prev(std::end(m_monstersInfo.reserve));
        pMonsterCage->type = monsterID;
    }

    // Add a monster in there
    pMonsterCage->monsters.emplace_back();
    auto& monsterInfo = pMonsterCage->monsters.back();
    monsterInfo.data.create(monsterID);

    Event event;
    event.type = "monster_added";
    event.monster.id = monsterID.c_str();
    EventEmitter::emit(event);
}

void Data::moveMonsterFromReserve(const sf::Vector2u& coords, const std::wstring& monsterID)
{
    returnif (!addMonsterValid(coords, monsterID));

    auto& reserve = m_monstersInfo.reserve;
    auto pMonsterCage = std::find_if(reserve, [&monsterID] (const MonsterCageInfo& monsterCage) { return monsterCage.type == monsterID; });

    // No cage in reserve concerning this monster
    returnif (pMonsterCage == std::end(reserve));

    // Check if enough monsters in cage
    auto monstersCount = pMonsterCage->monsters.size();
    returnif (monstersCount == 0u);

    // Now move
    auto& monsterInfo = pMonsterCage->monsters.back();
    monsterInfo.data[L"rx"].init_float(coords.x + 0.5f);
    monsterInfo.data[L"ry"].init_float(coords.y + 0.5f);
    m_monstersInfo.active.emplace_back(std::move(monsterInfo));
    pMonsterCage->monsters.resize(monstersCount - 1u);

    Event event;
    event.type = "monster_added";
    event.monster.id = monsterID.c_str();
    EventEmitter::emit(event);
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
        event.type = "mode_changed";
        event.mode = mode;
    }

    // For invasion, check dungeon validity.
    else if (mode == Mode::INVASION) {
        switch (m_graph->reconstructFromData()) {
        case Graph::ConstructError::NONE:
            m_mode = mode;
            event.type = "mode_changed";
            event.mode = mode;
            break;

        case Graph::ConstructError::NO_DOOR:
            event.type = "error";
            event.message = L"No door in the dungeon. How are heroes supposed to enter?";
            break;

        case Graph::ConstructError::TOO_MANY_DOORS:
            event.type = "error";
            event.message = L"Too many doors. Only one door in the dungeon is supported.";
            break;
        };
    }

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
