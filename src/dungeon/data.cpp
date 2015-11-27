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

    // Reload the monsters cages
    m_monstersInfo.reserve.reserve(m_monstersDB.get().size());
    for (const auto& monsterPair : m_monstersDB.get()) {
        MonsterCageInfo monsterCageInfo;
        monsterCageInfo.type = monsterPair.first;
        m_monstersInfo.reserve.emplace_back(std::move(monsterCageInfo));
    }
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
        EventEmitter::addEvent("time_changed");
    }

    // Monster reserve countdowns
    static float reserveTimeBuffer = 0.f;
    reserveTimeBuffer += dt.asSeconds();
    while (reserveTimeBuffer >= 1.f) {
        reserveTimeBuffer -= 1.f;

        for (auto& reserveInfo : m_monstersInfo.reserve) {
            if (reserveInfo.countdown == 0u) continue;
            reserveInfo.countdown -= 1u;

            auto event = std::make_unique<Event>();
            event->type = "reserve_countdown_changed";
            event->monster.id = reserveInfo.type.c_str();
            EventEmitter::addEvent(std::move(event));
        }
    }

    // Send events
    broadcast();

    // Heroes
    m_heroesManager.update(dt);
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
    m_fameWallet.set(dungeon.attribute(L"fame").as_uint());
    wdebug_dungeon_1(L"Dungeon is " << m_name << L" of size " << m_floorsCount << L"x" << m_roomsByFloor << L".");

    //---- Heroes

    const auto& heroesNode = dungeon.child(L"heroes");
    m_heroesManager.load(heroesNode);

    //---- Monsters

    // TODO Make a monstersManager too?

    // Reserve
    const auto& monstersNode = dungeon.child(L"monsters");
    const auto& reserveNode = monstersNode.child(L"reserve");
    for (const auto& monsterCageNode : reserveNode.children(L"monsterCage")) {
        std::wstring monsterID = monsterCageNode.attribute(L"type").as_string();
        auto found = std::find_if(m_monstersInfo.reserve, [&monsterID] (const MonsterCageInfo& monsterCageInfo) { return monsterCageInfo.type == monsterID; });

        // Ignore unknown monsterID
        if (found == std::end(m_monstersInfo.reserve))
            continue;

        auto& monsterCageInfo = *found;
        monsterCageInfo.countdown = monsterCageNode.attribute(L"countdown").as_uint();
        for (const auto& monsterNode : monsterCageNode.children(L"monster")) {
            MonsterInfo monsterInfo;
            monsterInfo.data.loadXML(monsterNode);
            monsterCageInfo.monsters.emplace_back(std::move(monsterInfo));
        }
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
                facilityInfo.barrier = facilityNode.attribute(L"barrier").as_bool();
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

    EventEmitter::addEvent("dungeon_changed");
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
    dungeon.append_attribute(L"fame") = m_fameWallet.value();

    //---- Heroes

    auto heroesNode = dungeon.append_child(L"heroes");
    m_heroesManager.save(heroesNode);

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
                if (facility.barrier) facilityNode.append_attribute(L"barrier") = true;
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

    EventEmitter::addEvent("dungeon_changed");
}

//-------------------//
//----- Emitter -----//

void Data::addEvent(std::string eventType, const sf::Vector2u& coords)
{
    auto event = std::make_unique<Event>();
    event->type = std::move(eventType);
    event->room = {coords.x, coords.y};
    EventEmitter::addEvent(std::move(event));
}

//-----------------//
//----- Rooms -----//

bool Data::isRoomConstructed(const sf::Vector2u& coords) const
{
    returnif (coords.x >= m_floorsCount) false;
    returnif (coords.y >= m_roomsByFloor) false;
    return (room(coords).state != RoomState::EMPTY);
}

bool Data::isRoomWalkable(const sf::Vector2u &coords) const
{
    returnif (coords.x >= m_floorsCount) false;
    returnif (coords.y >= m_roomsByFloor) false;
    const auto& selectedRoom = room(coords);
    returnif (selectedRoom.state == RoomState::EMPTY) false;

    // Check that there is no barrier in this room
    for (const auto& facility : selectedRoom.facilities)
        returnif (facility.barrier) false;

    return true;
}

void Data::constructRoom(const sf::Vector2u& coords)
{
    returnif (coords.x >= m_floorsCount);
    returnif (coords.y >= m_roomsByFloor);
    returnif (room(coords).state != RoomState::EMPTY);

    // Do construct
    room(coords).state = RoomState::CONSTRUCTED;
    createRoomFacilitiesLinks(coords);

    addEvent("room_constructed", coords);
    EventEmitter::addEvent("dungeon_changed");
}

void Data::destroyRoom(const sf::Vector2u& coords)
{
    returnif (!isRoomConstructed(coords));

    // Note: removeRoomFacilities() manage to remove also
    // implicit links if any silently, and that's all right.

    // Clear elements
    removeRoomFacilities(coords);
    removeRoomTrap(coords);
    removeRoomMonsters(coords);

    // Destroy the room
    room(coords).state = RoomState::EMPTY;

    addEvent("room_destroyed", coords);
    EventEmitter::addEvent("dungeon_changed");
}

bool Data::pushRoom(const sf::Vector2u& coords, Direction direction)
{
    returnif (coords.x >= m_floorsCount)  false;
    returnif (coords.y >= m_roomsByFloor) false;

    // We look for the first void in the direction,
    // if none, it's impossible to move the rooms
    auto voidCoords = coords;
    while (isRoomConstructed(voidCoords))
        voidCoords = roomNeighbourCoords(voidCoords, direction);

    returnif (voidCoords.x >= m_floorsCount)  false;
    returnif (voidCoords.y >= m_roomsByFloor) false;

    // We're all right, let's move the rooms
    auto antiDirection = oppositeDirection(direction);
    sf::Vector2u targetCoords = voidCoords;
    while (targetCoords != coords) {
        // Remove previous links if any (and explicit ones are broken)
        auto movingCoords = roomNeighbourCoords(targetCoords, antiDirection);
        removeRoomFacilitiesLinks(movingCoords);

        // Swapping the rooms
        auto& roomFrom = room(movingCoords);
        auto& roomTo = room(targetCoords);
        roomTo = std::move(roomFrom);

        // Reset coords
        roomFrom.coords = movingCoords;
        roomTo.coords = targetCoords;

        // Create the new links
        createRoomFacilitiesLinks(targetCoords);
        addEvent("room_changed", targetCoords);

        // Note: It is not our job to move the moving elements (monsters/heroes)
        // We let inter and the managers handle that
        targetCoords = movingCoords;
    }

    // Set the start room to void
    auto& startRoom = room(coords);
    startRoom.state = RoomState::EMPTY;
    removeRoomFacilitiesLinks(coords);
    addEvent("room_changed", coords);

    EventEmitter::addEvent("dungeon_changed");

    return true;
}

sf::Vector2u Data::roomNeighbourCoords(const sf::Vector2u& coords, Direction direction)
{
    return coords + roomDirectionVector2u(direction);
}

Data::Room& Data::roomNeighbour(const sf::Vector2u& coords, Direction direction)
{
    return room(roomNeighbourCoords(coords, direction));
}

sf::Vector2u Data::roomDirectionVector2u(Direction direction)
{
    return {(direction >> 0x4) - 1u, (direction & 0xf) - 1u};
}

sf::Vector2i Data::roomDirectionVector2i(Direction direction)
{
    return {(direction >> 0x4) - 1, (direction & 0xf) - 1};
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

    addEvent("facility_changed", coords);
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

    createFacilityLinks(coords, facility);
    addEvent("facility_changed", coords);

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
    auto pFacilityInfo = getFacility(coords, facilityID);
    returnif (pFacilityInfo == nullptr);

    pFacilityInfo->link = linkCoords;
    addEvent("facility_changed", coords);
}

void Data::createFacilityLinks(const sf::Vector2u& coords, const FacilityInfo& facility)
{
    const auto& facilityData = facilitiesDB().get(facility.data.type());
    for (const auto& link : facilityData.links) {
        // Create implicit links, ignore the explicit ones
        if (link.style == Link::Style::IMPLICIT) {
            sf::Vector2u linkCoords(coords.x + link.x, coords.y + link.y);
            createRoomFacility(linkCoords, link.id, true);
        }
    }
}

void Data::createRoomFacilitiesLinks(const sf::Vector2u& coords)
{
    returnif (!isRoomConstructed(coords));
    const auto& selectedRoom = room(coords);

    // For each facility of the room, create its links
    for (const auto& facility : selectedRoom.facilities)
        createFacilityLinks(coords, facility);

    // Note: The following code block seems a bit fat,
    // but beside keeping up-to-date a list of facilities
    // pointing to this room, there is no choice.
    // Implementing that selectedRoom.incomingLinks list would be awesome!
    // But be careful with the pushRoom() function moving memory as it wants.

    // We check all implicit links getting to this room
    // and create those which need to be there
    for (const auto& floor : m_floors)
    for (const auto& room : floor.rooms)
    for (const auto& facility : room.facilities) {
        const auto& facilityInfo = *facility.common;
        for (const auto& link : facilityInfo.links) {
            if (link.style == Link::Style::IMPLICIT) {
                sf::Vector2u linkCoords(room.coords.x + link.x, room.coords.y + link.y);
                if (linkCoords == coords)
                    createRoomFacility(coords, link.id, true);
            }
        }
    }
}

void Data::removeRoomFacilityLink(const sf::Vector2u& coords, const std::wstring& facilityID)
{
    auto pFacilityInfo = getFacility(coords, facilityID);
    returnif (pFacilityInfo == nullptr);

    pFacilityInfo->link = {-1u, -1u};
    addEvent("facility_changed", coords);
}

void Data::removeFacilityLinks(const sf::Vector2u& coords, FacilityInfo& facility)
{
    const auto& facilityData = facilitiesDB().get(facility.data.type());
    for (const auto& link : facilityData.links) {
        // Remove all implicit links
        if (link.style == Link::Style::IMPLICIT) {
            sf::Vector2u linkCoords(coords.x + link.x, coords.y + link.y);
            removeRoomFacility(linkCoords, link.id);
        }

        // Remove all explicit links of a change
        else if (link.style == Link::Style::EXPLICIT && (facility.link.x != -1u && facility.link.y != -1u)) {
            removeRoomFacilityLink(facility.link, link.id);
        }
    }

    // Also break this facility explicit link if any
    facility.link = {-1u, -1u};
}

void Data::removeRoomFacilitiesLinks(const sf::Vector2u& coords)
{
    returnif (!isRoomConstructed(coords));
    auto& selectedRoom = room(coords);

    // For each facility of the room, delete its links
    for (auto& facility : selectedRoom.facilities)
        removeFacilityLinks(coords, facility);

    // Also remove all implicit links in this room
    std::erase_if(selectedRoom.facilities, [] (const FacilityInfo& facility) { return facility.isLink; });
}

void Data::setRoomFacilityBarrier(const sf::Vector2u& coords, const std::wstring& facilityID, bool activated)
{
    auto pFacilityInfo = getFacility(coords, facilityID);
    returnif (pFacilityInfo == nullptr);

    pFacilityInfo->barrier = activated;
    addEvent("facility_changed", coords);
    addEvent("dungeon_changed", coords);
}

void Data::removeRoomFacility(const sf::Vector2u& coords, const std::wstring& facilityID)
{
    returnif (!isRoomConstructed(coords));

    auto& roomInfo = room(coords);
    auto pFacility = std::find_if(roomInfo.facilities, [facilityID] (const FacilityInfo& facilityInfo) { return facilityInfo.data.type() == facilityID; });
    returnif (pFacility == std::end(roomInfo.facilities));

    removeFacilityLinks(coords, *pFacility);
    roomInfo.facilities.erase(pFacility);

    addEvent("facility_changed", coords);
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

    addEvent("trap_changed", coords);
}

void Data::removeRoomTrap(const sf::Vector2u& coords)
{
    returnif (!isRoomConstructed(coords));

    auto& roomInfo = room(coords);
    returnif (!roomInfo.trap.exists());

    // TODO Use trapsDB data (in Inter!)
    // m_villain->doshWallet.add(traps::onDestroyGain(roomInfo.trap));
    roomInfo.trap.clear();

    addEvent("trap_changed", coords);
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
            EventEmitter::addEvent("monster_removed");
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
    auto& reserve = m_monstersInfo.reserve;
    auto pMonsterCage = std::find_if(reserve, [&monsterID] (const MonsterCageInfo& monsterCage) { return monsterCage.type == monsterID; });
    returnif (pMonsterCage == std::end(reserve));

    // Add a monster in there
    pMonsterCage->monsters.emplace_back();
    auto& monsterInfo = pMonsterCage->monsters.back();
    monsterInfo.data.create(monsterID);

    auto event = std::make_unique<Event>();
    event->type = "monster_added";
    event->monster.id = pMonsterCage->type.c_str();
    EventEmitter::addEvent(std::move(event));

    // Increase the countdown
    pMonsterCage->countdown += countdownIncrease;

    event = std::make_unique<Event>();
    event->type = "reserve_countdown_changed";
    event->monster.id = pMonsterCage->type.c_str();
    EventEmitter::addEvent(std::move(event));
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

    auto event = std::make_unique<Event>();
    event->type = "monster_added";
    event->monster.id = pMonsterCage->type.c_str();
    EventEmitter::addEvent(std::move(event));
}

//-------------------------//
//----- Graph linking -----//

void Data::useGraph(Graph& graph)
{
    m_graph = &graph;
    m_heroesManager.useGraph(graph);
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
