#include "dungeon/data.hpp"

#include "dungeon/debug.hpp"
#include "dungeon/graph.hpp"
#include "dungeon/elements/hero.hpp"
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
    // Game time
    static float gameTimeBuffer = 0.f;
    gameTimeBuffer += dt.asSeconds();
    while (gameTimeBuffer >= m_timeGameHour) {
        gameTimeBuffer -= m_timeGameHour;
        m_time += 1u;
        EventEmitter::emit("time_changed");
    }

    // Monster reserve countdowns
    static float reserveTimeBuffer = 0.f;
    reserveTimeBuffer += dt.asSeconds();
    while (reserveTimeBuffer >= 1.f) {
        reserveTimeBuffer -= 1.f;

        for (auto& reserveInfo : m_monstersInfo.reserve) {
            if (reserveInfo.countdown == 0u) continue;
            reserveInfo.countdown -= 1u;

            Event event;
            event.type = "reserve_countdown_changed";
            event.monster.id = reserveInfo.type.c_str();
            EventEmitter::emit(event);
        }
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
            if (roomStateString == L"void") room.state = RoomState::EMPTY;
            else if (roomStateString == L"constructed") room.state = RoomState::CONSTRUCTED;

            wdebug_dungeon_3(L"Found room " << room.pos << L" of state " << roomStateString);

            // Traps
            const auto& trapNode = roomNode.child(L"trap");
            if (trapNode) room.trap.loadXML(trapNode);

            // Facilities
            for (const auto& facilityNode : roomNode.children(L"facility")) {
                room.facilities.emplace_back();
                auto& facilityInfo = room.facilities.back();
                facilityInfo.data.loadXML(facilityNode);
                facilityInfo.isLink = facilityNode.attribute(L"isLink").as_bool();
                facilityInfo.common = &facilitiesDB().get(facilityInfo.data.type());
                facilityInfo.treasure = facilityNode.attribute(L"treasure").as_uint(-1u);
                facilityInfo.link.x = facilityNode.attribute(L"linkX").as_uint(-1u);
                facilityInfo.link.y = facilityNode.attribute(L"linkY").as_uint(-1u);

                // Tunnels
                for (auto tunnelNode : facilityNode.children(L"tunnel")) {
                    Tunnel tunnel;
                    tunnel.coords.x = tunnelNode.attribute(L"x").as_int();
                    tunnel.coords.y = tunnelNode.attribute(L"y").as_int();
                    tunnel.relative = tunnelNode.attribute(L"relative").as_bool();
                    facilityInfo.tunnels.emplace_back(std::move(tunnel));
                }
            }

            // Add the room
            m_floors[floorPos].rooms.emplace_back(std::move(room));
        }
    }

    EventEmitter::emit("dungeon_changed");
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
            if (roomState == RoomState::EMPTY) roomStateString = L"void";
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
                if (facility.isLink) facilityNode.append_attribute(L"isLink") = true;
                if (facility.treasure != -1u) facilityNode.append_attribute(L"treasure") = facility.treasure;
                if (facility.link.x != -1u) facilityNode.append_attribute(L"linkX") = facility.link.x;
                if (facility.link.y != -1u) facilityNode.append_attribute(L"linkY") = facility.link.y;
                facility.data.saveXML(facilityNode);

                // Tunnels
                for (const auto& tunnel : facility.tunnels) {
                    auto tunnelNode = facilityNode.append_child(L"tunnel");
                    tunnelNode.append_attribute(L"x") = tunnel.coords.x;
                    tunnelNode.append_attribute(L"y") = tunnel.coords.y;
                    if (tunnel.relative) tunnelNode.append_attribute(L"relative") = true;
                }
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
                ownRoom.state = RoomState::EMPTY;
        }
    }

    EventEmitter::emit("dungeon_changed");
}

//-------------------//
//----- Emitter -----//

void Data::emit(std::string eventType, const sf::Vector2u& coords)
{
    Event event;
    event.type = std::move(eventType);
    event.room = {coords.x, coords.y};
    EventEmitter::emit(event);
}

//-----------------//
//----- Rooms -----//

bool Data::isRoomConstructed(const sf::Vector2u& coords) const
{
    returnif (coords.x >= m_floorsCount) false;
    returnif (coords.y >= m_roomsByFloor) false;
    return (room(coords).state != RoomState::EMPTY);
}

void Data::constructRoom(const sf::Vector2u& coords, bool hard)
{
    returnif (coords.x >= m_floorsCount);
    returnif (coords.y >= m_roomsByFloor);
    returnif (room(coords).state != RoomState::EMPTY);

    // Do construct
    room(coords).state = RoomState::CONSTRUCTED;

    // We check all implicit links getting to this room
    // and create those which need to be there
    for (const auto& floor : m_floors)
    for (const auto& room : floor.rooms)
    for (const auto& facility : room.facilities) {
        const auto& facilityInfo = *facility.common;
        for (const auto& link : facilityInfo.links) {
            if (link.style == Link::Style::IMPLICIT) {
                sf::Vector2u linkCoords;
                linkCoords.x = room.coords.x + link.x;
                linkCoords.y = room.coords.y + link.y;
                if (linkCoords == coords)
                    createRoomFacility(coords, link.id, true);
            }
        }
    }

    emit("room_constructed", coords);
    EventEmitter::emit("dungeon_changed");
}

void Data::destroyRoom(const sf::Vector2u& coords)
{
    returnif (!isRoomConstructed(coords));

    // Clear elements
    removeRoomFacilities(coords);
    removeRoomTrap(coords);
    removeRoomMonsters(coords);

    // Destroy the room
    room(coords).state = RoomState::EMPTY;

    emit("room_destroyed", coords);
    EventEmitter::emit("dungeon_changed");
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
    uint treasureDosh = 0u;
    auto& roomInfo = room(coords);
    for (const auto& facilityInfo : roomInfo.facilities) {
        auto& treasure = facilityInfo.treasure;
        if (treasure == -1u) continue;
        treasureDosh += treasure;
    }
    return treasureDosh;
}

//----------------------------//
//----- Hero interaction -----//

// TODO This kind of high level function should probably not be in Data but in Inter
void Data::stealTreasure(const sf::Vector2u& coords, Hero& hero, uint stolenDosh)
{
    auto& roomInfo = room(coords);

    // Stealing all potential facilities until the amount of stolen dosh is reached
    for (auto& facilityInfo : roomInfo.facilities) {
        auto& treasure = facilityInfo.treasure;
        if (treasure == -1u) continue;

        auto dosh = std::min(treasure, stolenDosh);
        hero.edata()[L"dosh"].as_uint32() += dosh;
        treasure -= dosh;
        stolenDosh -= dosh;

        if (stolenDosh == 0u)
            break;
    }

    // Note: We could check that stolenDosh is zero here.
    // If not, we were not able to steal the amount asked.

    emit("facility_changed", coords);
}

//----------------------//
//----- Facilities -----//

bool Data::hasFacility(const sf::Vector2u& coords, const std::wstring& facilityID) const
{
    return getFacility(coords, facilityID) != nullptr;
}

FacilityInfo* Data::getFacility(const sf::Vector2u& coords, const std::wstring& facilityID)
{
    returnif (!isRoomConstructed(coords)) nullptr;
    auto& roomInfo = room(coords);
    auto found = std::find_if(roomInfo.facilities, [&facilityID] (const FacilityInfo& facilityInfo) { return facilityInfo.data.type() == facilityID; });
    returnif (found == std::end(roomInfo.facilities)) nullptr;
    return &(*found);
}

const FacilityInfo* Data::getFacility(const sf::Vector2u& coords, const std::wstring& facilityID) const
{
    returnif (!isRoomConstructed(coords)) nullptr;
    const auto& roomInfo = room(coords);
    auto found = std::find_if(roomInfo.facilities, [&facilityID] (const FacilityInfo& facilityInfo) { return facilityInfo.data.type() == facilityID; });
    returnif (found == std::end(roomInfo.facilities)) nullptr;
    return &(*found);
}

bool Data::createRoomFacility(const sf::Vector2u& coords, const std::wstring& facilityID, bool isLink)
{
    returnif (!isRoomConstructed(coords)) false;
    returnif (hasFacility(coords, facilityID)) false;

    // Facility creation, indeed
    auto& roomInfo = room(coords);
    roomInfo.facilities.emplace_back();
    auto& facility = roomInfo.facilities.back();
    facility.data.create(facilityID);
    facility.isLink = isLink;
    facility.common = &facilitiesDB().get(facilityID);

    emit("facility_changed", coords);

    // Create all the implicit links too
    const auto& facilityInfo = facilitiesDB().get(facilityID);
    for (const auto& link : facilityInfo.links) {
        if (link.style == Link::Style::IMPLICIT) {
            sf::Vector2u linkCoords;
            linkCoords.x = coords.x + link.x;
            linkCoords.y = coords.y + link.y;
            createRoomFacility(linkCoords, link.id, true);
        }
    }

    return true;
}

bool Data::createRoomFacilityLinked(const sf::Vector2u& coords, const std::wstring& facilityID, const sf::Vector2u& linkCoords, const std::wstring& linkFacilityID)
{
    returnif (!createRoomFacility(coords, facilityID)) false;
    returnif (!hasFacility(linkCoords, linkFacilityID)) true;

    // If both exist, link them together
    setRoomFacilityLink(coords, facilityID, linkCoords);
    setRoomFacilityLink(linkCoords, linkFacilityID, coords);
    return true;
}

void Data::setRoomFacilityLink(const sf::Vector2u& coords, const std::wstring& facilityID, const sf::Vector2u& linkCoords)
{
    returnif (!isRoomConstructed(coords));

    auto& roomInfo = room(coords);
    for (auto& facilityInfo : roomInfo.facilities) {
        if (facilityInfo.data.type() == facilityID) {
            facilityInfo.link = linkCoords;
            emit("facility_changed", coords);
            return;
        }
    }
}

void Data::removeRoomFacilityLink(const sf::Vector2u& coords, const std::wstring& facilityID)
{
    returnif (!isRoomConstructed(coords));

    auto& roomInfo = room(coords);
    for (auto& facilityInfo : roomInfo.facilities) {
        if (facilityInfo.data.type() == facilityID) {
            facilityInfo.link = {-1u, -1u};
            emit("facility_changed", coords);
            return;
        }
    }
}

void Data::removeRoomFacility(const sf::Vector2u& coords, const std::wstring& facilityID)
{
    returnif (!isRoomConstructed(coords));

    auto& roomInfo = room(coords);
    auto pFacility = std::find_if(roomInfo.facilities, [facilityID] (const FacilityInfo& facilityInfo) { return facilityInfo.data.type() == facilityID; });
    returnif (pFacility == std::end(roomInfo.facilities));

    // Check and update links
    const auto& facilityInfo = facilitiesDB().get(facilityID);
    for (const auto& link : facilityInfo.links) {
        // Remove all implicit links
        if (link.style == Link::Style::IMPLICIT) {
            sf::Vector2u linkCoords(coords.x + link.x, coords.y + link.y);
            // Note: Whatever happens, there can be only one of this ID
            // in that same room, so there no issue deleting it without more checking
            removeRoomFacility(linkCoords, link.id);
        }

        // Remove all explicit links of a change
        else if (link.style == Link::Style::EXPLICIT && (pFacility->link.x != -1u && pFacility->link.y != -1u)) {
            removeRoomFacilityLink(pFacility->link, link.id);
        }
    }

    roomInfo.facilities.erase(pFacility);

    emit("facility_changed", coords);
}

void Data::removeRoomFacilities(const sf::Vector2u& coords)
{
    returnif (!isRoomConstructed(coords));

    // Note: variable facilities here is a copy, so that progressive
    // removals won't change that.
    // A more optimized version could exist, but is it necessary?
    const auto& roomInfo = room(coords);
    const auto facilities = roomInfo.facilities;
    for (const auto& facility : facilities)
        if (!facility.isLink)
            removeRoomFacility(coords, facility.data.type());
}

//-----------------//
//----- Traps -----//

void Data::setRoomTrap(const sf::Vector2u& coords, const std::wstring& trapID)
{
    returnif (!isRoomConstructed(coords));

    auto& roomInfo = room(coords);
    returnif (roomInfo.trap.exists() && roomInfo.trap.type() == trapID);

    // Destroy previous trap if any and set it to the new one
    roomInfo.trap.clear();
    roomInfo.trap.create(trapID);

    emit("trap_changed", coords);
}

void Data::removeRoomTrap(const sf::Vector2u& coords)
{
    returnif (!isRoomConstructed(coords));

    auto& roomInfo = room(coords);
    returnif (!roomInfo.trap.exists());

    // TODO Use trapsDB data (in Inter!)
    // m_villain->doshWallet.add(traps::onDestroyGain(roomInfo.trap));
    roomInfo.trap.clear();

    emit("trap_changed", coords);
}

//--------------------//
//----- Monsters -----//

void Data::removeRoomMonsters(const sf::Vector2u& coords)
{
    std::erase_if(m_monstersInfo.active, [this, coords] (const MonsterInfo& monsterInfo) {
        sf::Vector2u monsterCoords;
        monsterCoords.x = static_cast<uint>(monsterInfo.data.at(L"rx").as_float());
        monsterCoords.y = static_cast<uint>(monsterInfo.data.at(L"ry").as_float());

        if (monsterCoords == coords) {
            EventEmitter::emit("monster_removed");
            return true;
        }

        return false;
    });
}

bool Data::addMonsterValid(const sf::Vector2u& coords, const std::wstring& monsterID)
{
    // TODO Why would we care about monsterID?
    return isRoomConstructed(coords);
}

void Data::addMonsterToReserve(const std::wstring& monsterID, const uint countdownIncrease)
{
    Event event;
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

    event.type = "monster_added";
    event.monster.id = monsterID.c_str();
    EventEmitter::emit(event);

    // Increase the countdown
    pMonsterCage->countdown += countdownIncrease;

    event.type = "reserve_countdown_changed";
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
