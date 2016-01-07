#include "dungeon/data.hpp"

#include "context/worlds.hpp"
#include "dungeon/debug.hpp"
#include "dungeon/graph.hpp"
#include "dungeon/elements/hero.hpp"
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
    // Wallets
    m_soulWallet.setEvents(this, "soul_changed");
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
        EventEmitter::addEvent("time_changed");

        // Debt
        // TODO We probably want to let the generalWallet to manage debts (and multiple debts)
        if (m_debtWeeksLeft > 0u) {
            auto weekSecond = m_time % (24u * 7u);
            if (weekSecond != 0u) continue;

            m_debtWeeksLeft -= 1u;
            Application::context().sounds.play("resources/interests");
            if (!doshWallet().sub(m_debtPerWeek)) {
                // TODO Game over if no more dosh
            }

            EventEmitter::addEvent("debt_changed");
        }
    }

    // Monster reserve countdowns
    static float reserveTimeBuffer = 0.f;
    reserveTimeBuffer += dt.asSeconds();
    while (reserveTimeBuffer >= 1.f) {
        reserveTimeBuffer -= 1.f;

        for (auto& monsterGenericPair : m_monstersGenerics) {
            if (monsterGenericPair.second.countdown == 0u) continue;
            monsterGenericPair.second.countdown -= 1u;

            auto event = std::make_unique<Event>();
            event->type = "reserve_countdown_changed";
            event->monster.id = monsterGenericPair.first.c_str();
            EventEmitter::addEvent(std::move(event));
        }
    }

    // Send events
    broadcast();

    // Managers
    m_heroesManager.update(dt);
    m_monstersManager.update(dt);
    m_dynamicsManager.update(dt);
}

//---------------------------//
//----- File management -----//

std::wstring Data::load(const std::wstring& folder)
{
    wdebug_dungeon_1(L"Loading data from folder " << folder);

    context::villains.load();
    m_villain = context::villains.getFromWorldFolder(folder);
    uint walletsFactor = (context::worlds.selected().gamemode == context::Gamemode::RICHMAN)? 0u : 1u;

    // We keep that case possible for tests.
    if (m_villain == nullptr) {
        std::wcout << L"/!\\ No villain found for folder '" << folder << "'." << std::endl;
        std::wcout << L" |  If the game crashes soon, that could be related." << std::endl;
        std::wcout << L" |  If you are running a test file, no problem here." << std::endl;
    }
    else {
        m_villain->doshWallet.setEvents(this, "dosh_changed");
        m_villain->doshWallet.setFactor(walletsFactor);
    }

    m_fameWallet.setFactor(walletsFactor);
    m_soulWallet.setFactor(walletsFactor);

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
    wdebug_dungeon_1(L"Dungeon is " << m_name << L" of size " << m_floorsCount << L"x" << m_roomsByFloor << L".");

    //---- Resources

    const auto& resourcesNode = dungeon.child(L"resources");
    m_soulWallet.set(resourcesNode.child(L"soul").attribute(L"value").as_uint());
    m_fameWallet.set(resourcesNode.child(L"fame").attribute(L"value").as_uint());
    const auto& debtNode = resourcesNode.child(L"debt");
    m_debtPerWeek = debtNode.attribute(L"perWeekDosh").as_uint();
    m_debtWeeksLeft = debtNode.attribute(L"weeksLeft").as_uint();

    //---- Dynamics

    const auto& dynamicsNode = dungeon.child(L"dynamics");
    m_dynamicsManager.load(dynamicsNode);

    //---- Heroes

    const auto& heroesNode = dungeon.child(L"heroes");
    m_heroesManager.load(heroesNode);

    //---- Monsters

    // Generics
    m_monstersGenerics.clear();
    const auto& monstersGenericsNode = dungeon.child(L"monstersGenerics");
    for (const auto& monsterData: m_monstersDB.get()) {
        const auto& monsterID = monsterData.first;
        const auto& monsterGenericNode = monstersGenericsNode.child(monsterID.c_str());
        auto& monsterGeneric = m_monstersGenerics[monsterData.first];
        monsterGeneric.common = &monsterData.second;
        monsterGeneric.unlocked = monsterGenericNode.attribute(L"unlocked").as_bool();
        monsterGeneric.reserve = monsterGenericNode.attribute(L"reserve").as_uint();
        monsterGeneric.countdown = monsterGenericNode.attribute(L"countdown").as_uint();
    }

    // Active
    const auto& monstersNode = dungeon.child(L"monsters");
    m_monstersManager.load(monstersNode);

    //---- Traps

    m_trapsGenerics.clear();
    const auto& trapsGenericsNode = dungeon.child(L"trapsGenerics");
    for (const auto& trapData: m_trapsDB.get()) {
        const auto& trapID = trapData.first;
        const auto& trapGenericNode = trapsGenericsNode.child(trapID.c_str());
        auto& trapGeneric = m_trapsGenerics[trapData.first];
        trapGeneric.common = &trapData.second;
        trapGeneric.unlocked = trapGenericNode.attribute(L"unlocked").as_bool();
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
            auto& trap = room.trap;
            const auto& trapNode = roomNode.child(L"trap");
            if (trapNode) {
                trap.data.loadXML(trapNode);
                trap.common = &trapsDB().get(trap.data.type());
            }

            // Facilities
            for (const auto& facilityNode : roomNode.children(L"facility")) {
                room.facilities.emplace_back();
                auto& facility= room.facilities.back();
                facility.data.loadXML(facilityNode);
                facility.isLink = facilityNode.attribute(L"isLink").as_bool();
                facility.barrier = facilityNode.attribute(L"barrier").as_bool();
                facility.common = &facilitiesDB().get(facility.data.type());
                facility.treasure = facilityNode.attribute(L"treasure").as_uint(-1u);
                facility.link.x = facilityNode.attribute(L"linkX").as_uint(-1u);
                facility.link.y = facilityNode.attribute(L"linkY").as_uint(-1u);
                room.hide |= facility.common->hide;

                // Tunnels
                for (auto tunnelNode : facilityNode.children(L"tunnel")) {
                    Tunnel tunnel;
                    tunnel.coords.x = tunnelNode.attribute(L"x").as_int();
                    tunnel.coords.y = tunnelNode.attribute(L"y").as_int();
                    tunnel.relative = tunnelNode.attribute(L"relative").as_bool();
                    facility.tunnels.emplace_back(std::move(tunnel));
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

    //---- Resources

    auto resourcesNode = dungeon.append_child(L"resources");
    resourcesNode.append_child(L"soul").append_attribute(L"value") = m_soulWallet.value();
    resourcesNode.append_child(L"fame").append_attribute(L"value") = m_fameWallet.value();
    auto debtNode = resourcesNode.append_child(L"debt");
    debtNode.append_attribute(L"perWeekDosh") = m_debtPerWeek;
    debtNode.append_attribute(L"weeksLeft") = m_debtWeeksLeft;

    //---- Dynamics

    auto dynamicsNode = dungeon.append_child(L"dynamics");
    m_dynamicsManager.save(dynamicsNode);

    //---- Heroes

    auto heroesNode = dungeon.append_child(L"heroes");
    m_heroesManager.save(heroesNode);

    //---- Monsters

    // Generics
    auto monstersGenericsNode = dungeon.append_child(L"monstersGenerics");
    for (const auto& monsterGenericPair : m_monstersGenerics) {
        auto monsterGenericNode = monstersGenericsNode.append_child(monsterGenericPair.first.c_str());
        monsterGenericNode.append_attribute(L"unlocked") = monsterGenericPair.second.unlocked;
        monsterGenericNode.append_attribute(L"reserve") = monsterGenericPair.second.reserve;
        monsterGenericNode.append_attribute(L"countdown") = monsterGenericPair.second.countdown;
    }

    // Active
    auto monstersNode = dungeon.append_child(L"monsters");
    m_monstersManager.save(monstersNode);

    //---- Traps

    auto trapsGenericsNode = dungeon.append_child(L"trapsGenerics");
    for (const auto& trapGenericPair : m_trapsGenerics) {
        auto trapGenericNode = trapsGenericsNode.append_child(trapGenericPair.first.c_str());
        trapGenericNode.append_attribute(L"unlocked") = trapGenericPair.second.unlocked;
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
            if (room.trap.data.exists()) {
                auto trapNode = roomNode.append_child(L"trap");
                room.trap.data.saveXML(trapNode);
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

bool Data::isRoomWalkable(const sf::Vector2u& coords) const
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

void Data::constructRoomsAll()
{
    for (auto& floor : m_floors)
    for (auto& room : floor.rooms)
        room.state = RoomState::CONSTRUCTED;
}

void Data::destroyRoom(const sf::Vector2u& coords)
{
    returnif (!isRoomConstructed(coords));

    // Clear elements
    removeRoomFacilitiesLinks(coords);
    removeRoomFacilities(coords);
    removeRoomTrap(coords);
    removeRoomMonsters(coords);

    // Destroy the room
    room(coords).state = RoomState::EMPTY;

    addEvent("room_destroyed", coords);
    EventEmitter::addEvent("dungeon_changed");

    updateRoomHide(coords);
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

    // Clean the starting room
    destroyRoom(coords);

    EventEmitter::addEvent("dungeon_changed");

    return true;
}

uint Data::stealRoomTreasure(const sf::Vector2u& coords, uint wantedDosh)
{
    auto& roomInfo = room(coords);

    // Stealing all potential facilities until the amount of stolen dosh is reached
    uint stolenDosh = 0u;
    for (auto& facilityInfo : roomInfo.facilities) {
        auto& treasure = facilityInfo.treasure;
        if (treasure == -1u) continue;

        auto dosh = std::min(treasure, wantedDosh);
        treasure -= dosh;
        wantedDosh -= dosh;
        stolenDosh += dosh;

        if (wantedDosh == 0u)
            break;
    }

    addEvent("facility_changed", coords);

    return stolenDosh;
}

sf::Vector2u Data::roomNeighbourCoords(const sf::Vector2u& coords, Direction direction)
{
    return coords + roomDirectionVector2u(direction);
}

Room& Data::roomNeighbour(const sf::Vector2u& coords, Direction direction)
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

void Data::updateRoomHide(const sf::Vector2u& coords)
{
    uint8 newHide = RoomFlag::NONE;

    auto& roomInfo = room(coords);
    if (roomInfo.state == RoomState::CONSTRUCTED)
        // Update the hide state from all facilities
        for (const auto& facilityInfo : roomInfo.facilities) {
            newHide |= facilityInfo.common->hide;
    }

    // Send an event if changed
    returnif (roomInfo.hide == newHide);
    roomInfo.hide = newHide;
    addEvent("room_hide_changed", coords);
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

bool Data::createRoomFacilityValid(const sf::Vector2u& coords, const std::wstring& facilityID)
{
    returnif (!isRoomConstructed(coords)) false;
    returnif (hasFacility(coords, facilityID)) false;
    const auto& selectedRoom = room(coords);
    const auto& facilityData = facilitiesDB().get(facilityID);

    // Check if no other facilities block the construction
    uint8 facilityLock = facilityData.lock;
    for (const auto& facility : selectedRoom.facilities)
        if ((facility.common->lock & facilityLock) != 0u)
            return false;

    // Check if the trap blocks the construction
    const auto& trap = selectedRoom.trap;
    if (trap.data.exists())
        if ((trap.common->lock & facilityLock) != 0u)
            return false;

    // Check againt absolute constraints for this facility
    bool excluded = false;
    for (const auto& constraint : facilityData.constraints) {
        // Are the coordinates concerned by this constraint?
        if (constraint.x.type == ConstraintParameter::Type::EQUAL)
            if (static_cast<uint>(constraint.x.value) != coords.x) continue;

        if (constraint.y.type == ConstraintParameter::Type::EQUAL)
            if (static_cast<uint>(constraint.y.value) != coords.y) continue;

        // If in include list, that's all right
        if (constraint.mode == Constraint::Mode::INCLUDE)
            return true;
        // If in exclude list, wait till the end (it might be in include list too)
        else if (constraint.mode == Constraint::Mode::EXCLUDE)
            excluded = true;
    }

    return !excluded;
}

bool Data::createRoomFacility(const sf::Vector2u& coords, const std::wstring& facilityID, bool isLink)
{
    returnif (!createRoomFacilityValid(coords, facilityID)) false;

    // Facility creation, indeed
    auto& roomInfo = room(coords);
    roomInfo.facilities.emplace_back();
    auto& facility = roomInfo.facilities.back();
    facility.data.create(facilityID);
    facility.isLink = isLink;
    facility.common = &facilitiesDB().get(facilityID);


    addEvent("facility_changed", coords);
    if (facility.common->entrance)
        EventEmitter::addEvent("dungeon_changed");

    updateRoomHide(coords);

    // Keep last, as this function could change the pending reference to facility
    // by creating a implicit link in the same room we are.
    createFacilityLinks(coords, facility);

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
    const auto& facilityData = *facility.common;
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
    const auto& selectedRoomFacilities = selectedRoom.facilities;

    // For each facility in the room, create its links
    // Note: as a facility implicit could create a facility in that same room,
    // we need to keep the original count for this facility.
    auto facilitiesCount = selectedRoomFacilities.size();
    for (uint i = 0u; i < facilitiesCount; ++i)
        createFacilityLinks(coords, selectedRoomFacilities[i]);

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
    EventEmitter::addEvent("dungeon_changed");
}

void Data::setRoomFacilityTreasure(const sf::Vector2u& coords, const std::wstring& facilityID, uint32 amount)
{
    auto pFacilityInfo = getFacility(coords, facilityID);
    returnif (pFacilityInfo == nullptr);

    pFacilityInfo->treasure = amount;
    addEvent("treasure_changed", coords);
}

void Data::addFacilityTunnel(FacilityInfo& facilityInfo, const sf::Vector2i& tunnelCoords, bool relative)
{
    Tunnel tunnel;
    tunnel.coords = tunnelCoords;
    tunnel.relative = relative;
    facilityInfo.tunnels.emplace_back(std::move(tunnel));

    EventEmitter::addEvent("dungeon_changed");
}

void Data::removeRoomFacility(const sf::Vector2u& coords, const std::wstring& facilityID)
{
    returnif (!isRoomConstructed(coords));

    auto& roomInfo = room(coords);
    auto pFacility = std::find_if(roomInfo.facilities, [facilityID] (const FacilityInfo& facilityInfo) { return facilityInfo.data.type() == facilityID; });
    returnif (pFacility == std::end(roomInfo.facilities));

    bool dungeonChanged = pFacility->common->entrance;
    dungeonChanged |= !pFacility->tunnels.empty();
    dungeonChanged |= pFacility->barrier;

    bool treasureChanged = pFacility->treasure != -1u;

    removeFacilityLinks(coords, *pFacility);
    roomInfo.facilities.erase(pFacility);

    // Events
    addEvent("facility_changed", coords);
    if (dungeonChanged)     EventEmitter::addEvent("dungeon_changed");
    if (treasureChanged)    EventEmitter::addEvent("treasure_changed");

    updateRoomHide(coords);
}

void Data::removeRoomFacilities(const sf::Vector2u& coords)
{
    returnif (!isRoomConstructed(coords));

    // OPTIM The variable facilities here is a copy, so that progressive
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
    returnif (roomInfo.trap.data.exists() && roomInfo.trap.data.type() == trapID);

    // Destroy previous trap if any
    roomInfo.trap.data.clear();

    // And and set it to the new one
    roomInfo.trap.data.create(trapID);
    roomInfo.trap.common = &m_trapsDB.get(trapID);

    // TODO Have a setRoomTrapValid()
    // that checks for the lock

    addEvent("trap_changed", coords);
}

void Data::removeRoomTrap(const sf::Vector2u& coords)
{
    returnif (!isRoomConstructed(coords));

    auto& roomInfo = room(coords);
    returnif (!roomInfo.trap.data.exists());
    roomInfo.trap.data.clear();

    addEvent("trap_changed", coords);
}

void Data::setTrapGenericUnlocked(const std::wstring& trapID, bool unlocked)
{
    m_trapsGenerics[trapID].unlocked = unlocked;
    EventEmitter::addEvent("trap_generic_changed");
}

//--------------------//
//----- Monsters -----//

void Data::removeRoomMonsters(const sf::Vector2u& coords)
{
    m_monstersManager.removeRoomMonsters(coords);
}

bool Data::addMonsterValid(const sf::Vector2u& coords, const std::wstring& monsterID)
{
    // TODO Why would we care about monsterID?
    return isRoomConstructed(coords);
}

void Data::addMonsterToReserve(const std::wstring& monsterID, const uint countdownIncrease)
{
    auto monsterGenericPair = m_monstersGenerics.find(monsterID);
    returnif (monsterGenericPair == std::end(m_monstersGenerics));

    // Reserve
    monsterGenericPair->second.reserve += 1u;

    auto event = std::make_unique<Event>();
    event->type = "monster_added";
    event->monster.id = monsterGenericPair->first.c_str();
    EventEmitter::addEvent(std::move(event));

    // Increase the countdown
    monsterGenericPair->second.countdown += countdownIncrease;

    event = std::make_unique<Event>();
    event->type = "reserve_countdown_changed";
    event->monster.id = monsterGenericPair->first.c_str();
    EventEmitter::addEvent(std::move(event));
}

void Data::moveMonsterFromReserve(const sf::Vector2u& coords, const std::wstring& monsterID)
{
    returnif (!addMonsterValid(coords, monsterID));

    auto monsterGenericPair = m_monstersGenerics.find(monsterID);
    returnif (monsterGenericPair == std::end(m_monstersGenerics));
    returnif (monsterGenericPair->second.reserve == 0u);

    // Add the monster
    m_monstersManager.addRoomMonster(coords, monsterID);

    auto event = std::make_unique<Event>();
    event->type = "monster_added";
    event->monster.id = monsterGenericPair->first.c_str();
    EventEmitter::addEvent(std::move(event));

    // Reserve
    monsterGenericPair->second.reserve -= 1u;

    event = std::make_unique<Event>();
    event->type = "reserve_countdown_changed";
    event->monster.id = monsterGenericPair->first.c_str();
    EventEmitter::addEvent(std::move(event));
}

void Data::setMonsterGenericUnlocked(const std::wstring& monsterID, bool unlocked)
{
    m_monstersGenerics[monsterID].unlocked = unlocked;
    EventEmitter::addEvent("monster_generic_changed");
}

//---------------------//
//----- Resources -----//

void Data::setDebt(uint debtPerWeek, uint debtWeeksDuration)
{
    m_debtPerWeek = debtPerWeek;
    m_debtWeeksLeft = debtWeeksDuration;
}

//-------------------------//
//----- Graph linking -----//

void Data::useGraph(Graph& graph)
{
    m_graph = &graph;
    m_heroesManager.useGraph(graph);
    m_monstersManager.useGraph(graph);
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
