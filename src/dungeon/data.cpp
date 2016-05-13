#include "dungeon/data.hpp"

#include "context/context.hpp"
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
    , m_floorRoomsCount(0u)
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
            context::context.sounds.play("core/resources/bank/interests");
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

    // We keep that case possible for tests.
    if (m_villain == nullptr) {
        std::wcout << L"/!\\ No villain found for folder '" << folder << "'." << std::endl;
        std::wcout << L" |  If the game crashes soon, that could be related." << std::endl;
        std::wcout << L" |  If you are running a test file, no problem here." << std::endl;
    }
    else {
        uint walletsFactor = (context::worlds.selected().gamemode == context::Gamemode::RICHMAN)? 0u : 1u;
        m_villain->doshWallet.setEvents(this, "dosh_changed");
        m_villain->doshWallet.setFactor(walletsFactor);
        m_villain->evilWallet.setEvents(this, "evil_changed");
        m_villain->evilWallet.setFactor(walletsFactor);
        m_fameWallet.setFactor(walletsFactor);
        m_soulWallet.setFactor(walletsFactor);
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

    // Parsing XML
    pugi::xml_document doc;
    doc.load_file(file.c_str());

    const auto& dungeon = doc.child(L"dungeon");

    if (!dungeon)
        mquit("File " + toString(file) + " is not a valid dungeon file.");

    //---- Dungeon

    m_name = dungeon.attribute(L"name").as_string();
    m_time = dungeon.attribute(L"time").as_uint();
    m_floorsCount = dungeon.attribute(L"floorsCount").as_uint();
    m_floorRoomsCount = dungeon.attribute(L"floorRoomsCount").as_uint();
    wdebug_dungeon_1(L"Dungeon is " << m_name << L" of size " << m_floorsCount << L"x" << m_floorRoomsCount << L".");

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

        m_floors.emplace_back();
        m_floors.back().pos = floorPos;
        mdebug_dungeon_2("Found floor " << floorPos);

        // Rooms
        m_floors[floorPos].rooms.reserve(m_floorRoomsCount);
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
                trap.barrier = trapNode.attribute(L"barrier").as_bool();
                trap.common = &trapsDB().get(trap.data.type());
            }

            // Facilities
            for (const auto& facilityNode : roomNode.children(L"facility")) {
                room.facilities.emplace_back();
                auto& facility = room.facilities.back();
                facility.coords = room.coords;
                facility.data.loadXML(facilityNode);
                facility.stronglyLinked = facilityNode.attribute(L"stronglyLinked").as_bool();
                facility.barrier = facilityNode.attribute(L"barrier").as_bool();
                facility.common = &facilitiesDB().get(facility.data.type());
                facility.treasure = facilityNode.attribute(L"treasure").as_uint(-1u);
                room.hide |= facility.common->hide;

                // Tunnels
                for (auto tunnelNode : facilityNode.children(L"tunnel")) {
                    Tunnel tunnel;
                    tunnel.coords.x = tunnelNode.attribute(L"x").as_int();
                    tunnel.coords.y = tunnelNode.attribute(L"y").as_int();
                    tunnel.relative = tunnelNode.attribute(L"relative").as_bool();
                    facility.tunnels.emplace_back(std::move(tunnel));
                }

                // Links
                for (auto linkNode : facilityNode.children(L"link")) {
                    FacilityLink link;
                    link.coords.x = linkNode.attribute(L"x").as_uint();
                    link.coords.y = linkNode.attribute(L"y").as_uint();
                    link.relink = linkNode.attribute(L"relink").as_bool();
                    link.common = facility.common->linkFind(linkNode.attribute(L"id").as_uint());
                    facility.links.emplace_back(std::move(link));
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
    dungeon.append_attribute(L"floorRoomsCount") = m_floorRoomsCount;

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
            const auto& trap = room.trap;
            if (trap.data.exists()) {
                auto trapNode = roomNode.append_child(L"trap");
                if (trap.barrier) trapNode.append_attribute(L"barrier") = true;
                trap.data.saveXML(trapNode);
            }

            // Facilities
            for (const auto& facility : room.facilities) {
                auto facilityNode = roomNode.append_child(L"facility");
                if (facility.stronglyLinked) facilityNode.append_attribute(L"stronglyLinked") = true;
                if (facility.barrier) facilityNode.append_attribute(L"barrier") = true;
                if (facility.treasure != -1u) facilityNode.append_attribute(L"treasure") = facility.treasure;
                facility.data.saveXML(facilityNode);

                // Tunnels
                for (const auto& tunnel : facility.tunnels) {
                    auto tunnelNode = facilityNode.append_child(L"tunnel");
                    tunnelNode.append_attribute(L"x") = tunnel.coords.x;
                    tunnelNode.append_attribute(L"y") = tunnel.coords.y;
                    if (tunnel.relative) tunnelNode.append_attribute(L"relative") = true;
                }

                // Links
                for (const auto& link : facility.links) {
                    auto linkNode = facilityNode.append_child(L"link");
                    if (link.common != nullptr) linkNode.append_attribute(L"id") = link.common->id;
                    if (link.relink) linkNode.append_attribute(L"relink") = true;
                    linkNode.append_attribute(L"x") = link.coords.x;
                    linkNode.append_attribute(L"y") = link.coords.y;
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
        floor.rooms.resize(m_floorRoomsCount);
        for (uint roomPos = 0; roomPos < m_floorRoomsCount; ++roomPos) {
            auto& ownRoom = floor.rooms[roomPos];
            ownRoom.coords.x = floorPos;
            ownRoom.coords.y = roomPos;

            // Unknown rooms become empty
            if (ownRoom.state == RoomState::UNKNOWN)
                ownRoom.state = RoomState::EMPTY;
        }
    }

    EventEmitter::addEvent("dungeon_structure_changed", true);
}

//-------------------//
//----- Emitter -----//

void Data::addEvent(std::string eventType, const RoomCoords& coords)
{
    auto event = std::make_unique<Event>();
    event->type = std::move(eventType);
    event->room = {coords.x, coords.y};
    EventEmitter::addEvent(std::move(event));
}

//-----------------//
//----- Rooms -----//

bool Data::isRoomConstructed(const RoomCoords& coords) const
{
    returnif (coords.x >= m_floorsCount) false;
    returnif (coords.y >= m_floorRoomsCount) false;
    return (room(coords).state != RoomState::EMPTY);
}

bool Data::isRoomWalkable(const RoomCoords& coords) const
{
    returnif (coords.x >= m_floorsCount) false;
    returnif (coords.y >= m_floorRoomsCount) false;
    const auto& selectedRoom = room(coords);
    returnif (selectedRoom.state == RoomState::EMPTY) false;

    // Check that there is no barrier in this room
    for (const auto& facility : selectedRoom.facilities)
        returnif (facility.barrier) false;
    returnif (selectedRoom.trap.data.exists() && selectedRoom.trap.barrier) false;

    return true;
}

void Data::constructRoom(const RoomCoords& coords)
{
    returnif (coords.x >= m_floorsCount);
    returnif (coords.y >= m_floorRoomsCount);
    returnif (room(coords).state != RoomState::EMPTY);

    // Do construct
    room(coords).state = RoomState::CONSTRUCTED;
    roomLinksIncomingStrongRecreateFacilities(coords);

    addEvent("room_constructed", coords);
    EventEmitter::addEvent("dungeon_changed");
}

void Data::constructRoomsAll()
{
    for (auto& floor : m_floors)
    for (auto& room : floor.rooms)
        room.state = RoomState::CONSTRUCTED;
}

void Data::destroyRoom(const RoomCoords& coords)
{
    returnif (!isRoomConstructed(coords));

    // Clear elements
    facilitiesRemove(coords, true);
    removeRoomTrap(coords);
    removeRoomMonsters(coords);

    // Destroy the room
    room(coords).state = RoomState::EMPTY;

    addEvent("room_destroyed", coords);
    EventEmitter::addEvent("dungeon_changed");

    updateRoomHide(coords);
}

bool Data::pushRoom(const RoomCoords& coords, Direction direction)
{
    returnif (coords.x >= m_floorsCount)  false;
    returnif (coords.y >= m_floorRoomsCount) false;

    // We look for the first void in the direction,
    // if none, it's impossible to move the rooms
    auto voidCoords = coords;
    while (isRoomConstructed(voidCoords))
        voidCoords = roomNeighbourCoords(voidCoords, direction);

    returnif (voidCoords.x >= m_floorsCount)  false;
    returnif (voidCoords.y >= m_floorRoomsCount) false;

    // We're all right, let's move the rooms
    auto antiDirection = oppositeDirection(direction);
    RoomCoords targetCoords = voidCoords;
    while (targetCoords != coords) {
        // Remove previous links if any
        auto movingCoords = roomNeighbourCoords(targetCoords, antiDirection);
        roomLinksStrongRemoveFacilities(movingCoords);
        roomLinksIncomingStrongRemoveFacilities(movingCoords);
        roomLinksIncomingRemove(movingCoords);
        roomLinksBreakableRemove(movingCoords);

        // Swapping the rooms
        auto& roomFrom = room(movingCoords);
        auto& roomTo = room(targetCoords);
        roomTo = std::move(roomFrom);

        // Reset coords
        roomFrom.coords = movingCoords;
        roomTo.coords = targetCoords;
        for (auto& facilityInfo : roomTo.facilities)
            facilityInfo.coords = targetCoords;

        // Create the new links
        roomLinksIncomingStrongRecreateFacilities(targetCoords);
        roomLinksStrongRecreateFacilities(targetCoords);
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

uint Data::stealRoomTreasure(const RoomCoords& coords, uint wantedDosh)
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

RoomCoords Data::roomNeighbourCoords(const RoomCoords& coords, Direction direction)
{
    return coords + roomDirectionVector2u(direction);
}

Room& Data::roomNeighbour(const RoomCoords& coords, Direction direction)
{
    return room(roomNeighbourCoords(coords, direction));
}

RoomCoords Data::roomDirectionVector2u(Direction direction)
{
    return {static_cast<uint8>((direction >> 0x4) - 1_u8), static_cast<uint8>((direction & 0xf) - 1_u8)};
}

RoomDirection Data::roomDirectionVector2i(Direction direction)
{
    return {static_cast<int8>((direction >> 0x4) - 1_i8), static_cast<int8>((direction & 0xf) - 1_i8)};
}

uint Data::roomTreasureDosh(const RoomCoords& coords)
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

void Data::updateRoomHide(const RoomCoords& coords)
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

uint8 Data::roomLock(const RoomCoords& coords, bool withPermissive, bool withTrap) const
{
    uint8 lock = RoomFlag::NONE;
    const auto& roomInfo = room(coords);

    // Facilities
    for (const auto& facility : roomInfo.facilities)
        if (withPermissive || !facility.common->permissive)
            lock |= facility.common->lock;

    // Trap
    if (withTrap) {
        const auto& trap = roomInfo.trap;
        if (trap.data.exists())
            lock |= trap.common->lock;
    }

    return lock;
}

//----------------------//
//----- Facilities -----//

bool Data::hasFacility(const RoomCoords& coords, const std::wstring& facilityID) const
{
    return facilitiesFind(coords, facilityID) != nullptr;
}

FacilityInfo* Data::facilitiesFind(const RoomCoords& coords, const std::wstring& facilityID)
{
    returnif (!isRoomConstructed(coords)) nullptr;
    auto& roomInfo = room(coords);
    auto found = std::find_if(roomInfo.facilities, [&facilityID] (const FacilityInfo& facilityInfo) { return facilityInfo.data.type() == facilityID; });
    returnif (found == std::end(roomInfo.facilities)) nullptr;
    return &(*found);
}

const FacilityInfo* Data::facilitiesFind(const RoomCoords& coords, const std::wstring& facilityID) const
{
    returnif (!isRoomConstructed(coords)) nullptr;
    const auto& roomInfo = room(coords);
    auto found = std::find_if(roomInfo.facilities, [&facilityID] (const FacilityInfo& facilityInfo) { return facilityInfo.data.type() == facilityID; });
    returnif (found == std::end(roomInfo.facilities)) nullptr;
    return &(*found);
}

bool Data::createRoomFacilityValid(const RoomCoords& coords, const std::wstring& facilityID)
{
    returnif (!isRoomConstructed(coords)) false;
    returnif (hasFacility(coords, facilityID)) false;

    // Check lock validity
    const auto& facilityData = facilitiesDB().get(facilityID);
    auto facilityPrint = facilityData.lock & roomLock(coords, false, true);
    returnif (facilityPrint != 0_u8) false;

    // Check againt absolute constraints for this facility
    return !constraintsExclude(facilityData.constraints, coords);
}

bool Data::facilitiesCreate(const RoomCoords& coords, const std::wstring& facilityID)
{
    returnif (!createRoomFacilityValid(coords, facilityID)) false;
    auto& roomInfo = room(coords);
    const auto& facilityData = facilitiesDB().get(facilityID);

    // Note: This event needs to be before the permissive removals,
    //       so that it is correctly queued for dungeon::Inter
    //       (fixing a bug where the lua of ladder was not aware of the ladderExit removal)
    addEvent("facility_changed", coords);

    // Send the warnings
    // TODO Make a specific event for the warnings?
    const auto& warnings = facilityData.warnings;
    warningsSend(warnings, coords, "facility_changed");

    // Remove permissive facilities that are in the way
    facilitiesPermissiveRemoveLocking(coords, facilityData.lock);

    // Facility creation, indeed
    roomInfo.facilities.emplace_back();
    auto& facility = roomInfo.facilities.back();
    facility.data.create(facilityID);
    facility.common = &facilityData;
    facility.coords = coords;

    if (facility.common->entrance)
        EventEmitter::addEvent("dungeon_changed");

    updateRoomHide(coords);

    // Keep last, as this function could change the pending reference to facility
    // by creating another facility in the same room we are
    facilityLinksStrongRecreateFacilities(facility);

    return true;
}

void Data::facilitiesRemove(const RoomCoords& coords, const std::wstring& facilityID, bool evenStronglyLinked)
{
    returnif (!isRoomConstructed(coords));
    auto& roomInfo = room(coords);

    // Note: This event needs to be before the incoming removals,
    //       so that it is correctly queued for dungeon::Inter
    addEvent("facility_changed", coords);

    auto pFacility = std::find_if(roomInfo.facilities, [facilityID] (const FacilityInfo& facilityInfo) { return facilityInfo.data.type() == facilityID; });
    returnif (pFacility == std::end(roomInfo.facilities));
    returnif (!evenStronglyLinked && pFacility->stronglyLinked);

    // Send the warnings
    const auto& warnings = pFacility->common->warnings;
    warningsSend(warnings, coords, "facility_changed");

    // Event checking
    bool dungeonChanged = pFacility->common->entrance;
    dungeonChanged |= !pFacility->tunnels.empty();
    dungeonChanged |= pFacility->barrier;

    bool treasureChanged = pFacility->treasure != -1u;

    if (dungeonChanged)     EventEmitter::addEvent("dungeon_changed");
    if (treasureChanged)    addEvent("treasure_changed", coords);

    // Complete removal
    facilityLinksIncomingRemove(coords, facilityID);
    facilityLinksStrongRemoveFacilities(*pFacility);
    roomInfo.facilities.erase(pFacility);

    // Removing a facility could allow a strongly linked one to be recreated
    if (!evenStronglyLinked)
        roomLinksIncomingStrongRecreateFacilities(coords);

    updateRoomHide(coords);
}

void Data::facilitiesRemove(const RoomCoords& coords, bool evenStronglyLinked)
{
    returnif (!isRoomConstructed(coords));

    // OPTIM The variable facilities here is a copy, so that progressive
    // removals won't change what's to remove.
    // A more optimized version could exist, but is it necessary?
    const auto& roomInfo = room(coords);
    const auto facilities = roomInfo.facilities;
    for (const auto& facility : facilities)
        if (evenStronglyLinked || !facility.stronglyLinked)
            facilitiesRemove(coords, facility.data.type(), true);

    // Just be sure to recreate all incoming strong links
    if (!evenStronglyLinked)
        roomLinksIncomingStrongRecreateFacilities(coords);
}

void Data::facilitiesPermissiveRemoveLocking(const RoomCoords& coords, uint8 lock)
{
    auto& roomInfo = room(coords);
    auto facilities = roomInfo.facilities;
    for (auto& facility : facilities) {
        if (!facility.common->permissive) continue;
        if ((facility.common->lock & lock) == 0u) continue;
        facilitiesRemove(coords, facility.data.type(), true);
    }
}

//----- Warnings

void Data::warningsSend(const std::vector<Warning>& warnings, const RoomCoords& coords, const std::string& eventType)
{
    for (const auto& warning : warnings) {
        RoomCoords warningCoords{sf::v2u8(warning.coords)};
        if (warning.relative) warningCoords += coords;
        if (coords.x >= m_floorsCount || coords.y >= m_floorRoomsCount) continue;
        addEvent(eventType, warningCoords);
    }
}

//----- Links

void Data::facilityLinksAdd(const RoomCoords& coords, const std::wstring& facilityID, const Link* common, const RoomCoords& linkCoords, bool relink)
{
    auto pFacility = facilitiesFind(coords, facilityID);
    returnif (pFacility == nullptr);
    facilityLinksAdd(*pFacility, common, linkCoords, relink);
}

void Data::facilityLinksAdd(FacilityInfo& facilityInfo, const Link* common, const RoomCoords& linkCoords, bool relink)
{
    facilityInfo.links.emplace_back();
    auto& link = facilityInfo.links.back();
    link.common = common;
    link.coords = linkCoords;
    link.relink = relink;
    if (common != nullptr) link.id = common->id;
    addEvent("facility_changed", facilityInfo.coords);
}

void Data::facilityLinkRedirect(FacilityInfo& facilityInfo, const uint8 id, const RoomCoords& linkCoords)
{
    for (auto& link : facilityInfo.links) {
        if (link.id != id) continue;
        link.coords = linkCoords;
        addEvent("facility_changed", facilityInfo.coords);
        return;
    }
}

void Data::facilityLinkLastSetID(const RoomCoords& coords, const std::wstring& facilityID, uint8 id)
{
    auto pFacility = facilitiesFind(coords, facilityID);
    returnif (pFacility == nullptr);
    facilityLinkLastSetID(*pFacility, id);
}

void Data::facilityLinkLastSetID(FacilityInfo& facilityInfo, uint8 id)
{
    auto& link = facilityInfo.links.back();
    link.id = id;
    addEvent("facility_changed", facilityInfo.coords);
}

void Data::facilityLinksRemove(const RoomCoords& coords, const std::wstring& facilityID, const RoomCoords& linkCoords, const std::wstring& linkFacilityID)
{
    auto pFacility = facilitiesFind(coords, facilityID);
    returnif (pFacility == nullptr);
    facilityLinksRemove(*pFacility, linkCoords, linkFacilityID);
}

void Data::facilityLinksRemove(FacilityInfo& facilityInfo, const RoomCoords& linkCoords, const std::wstring& linkFacilityID)
{
    std::erase_if(facilityInfo.links, [&linkCoords, &linkFacilityID] (const FacilityLink& link)
                  { return link.coords == linkCoords && link.common->facilityID == linkFacilityID; });
    addEvent("facility_changed", facilityInfo.coords);
}

void Data::facilityLinksRemove(FacilityInfo& facilityInfo, uint8 id)
{
    auto iLink = std::find_if(facilityInfo.links, [id] (const FacilityLink& link) { return link.id == id; });
    if (iLink == std::end(facilityInfo.links)) return;

    // We might need to remove a relinked link
    if (iLink->common != nullptr && iLink->common->relink)
        facilityLinksRemove(iLink->coords, iLink->common->facilityID, facilityInfo.coords, facilityInfo.data.type());

    facilityInfo.links.erase(iLink);
    addEvent("facility_changed", facilityInfo.coords);
}

void Data::facilityLinksStrongRemoveFacilities(FacilityInfo& facilityInfo)
{
    // Note: Intended copy of the links because of the removals
    auto links = facilityInfo.links;
    for (auto& link : links)
        if (link.common != nullptr && link.common->strong)
            facilitiesRemove(link.coords, link.common->facilityID, true);
}

void Data::facilityLinksIncomingRemove(const RoomCoords& coords, const std::wstring& facilityID)
{
    for (auto& floor : m_floors)
    for (auto& room : floor.rooms)
    for (auto& facility : room.facilities) {
        auto links = facility.links;
        for (auto& link : links) {
            if (link.coords != coords) continue;
            if (link.common == nullptr || link.common->facilityID.empty()) continue;
            if (!link.relink && link.common->facilityID != facilityID) continue;
            else if (link.relink && link.common->originFacilityID != facilityID) continue;
            facilityLinksRemove(facility, coords, facilityID);
        }
    }
}

void Data::facilityLinksStrongRecreateFacilities(FacilityInfo& facility)
{
    auto coords = facility.coords;
    auto facilityID = facility.data.type();

    const auto& facilityData = *facility.common;
    for (const auto& link : facilityData.fixedLinks) {
        if (link.facilityID.empty()) continue;
        if (!link.strong) continue;

        RoomCoords linkCoords{static_cast<uint8>(link.coords.x), static_cast<uint8>(link.coords.y)};
        if (link.relative) linkCoords += coords;
        bool success = facilitiesCreate(linkCoords, link.facilityID);
        if (!success) continue;

        // Our facility registers a new link
        auto pFacility = facilitiesFind(linkCoords, link.facilityID);
        pFacility->stronglyLinked = true;
        facilityLinksAdd(coords, facilityID, &link, linkCoords);
    }
}

void Data::roomLinksIncomingStrongRecreateFacilities(const RoomCoords& coords)
{
    returnif (!isRoomConstructed(coords));

    // Note: The following code block seems a bit fat,
    // but beside keeping up-to-date a list of facilities
    // pointing to this room, there is no choice.
    // Implementing that selectedRoom.incomingLinks list would be awesome!
    // But be careful with the pushRoom() function moving memory as it wants.

    // We check all links getting to this room
    // and create those which need to be there
    for (auto& floor : m_floors)
    for (auto& room : floor.rooms)
    for (auto& facility : room.facilities)
    for (const auto& link : facility.common->fixedLinks) {
        if (link.facilityID.empty()) continue;
        if (!link.strong) continue;

        RoomCoords linkCoords{static_cast<uint8>(link.coords.x), static_cast<uint8>(link.coords.y)};
        if (link.relative) linkCoords += room.coords;
        if (linkCoords != coords) continue;
        bool success = facilitiesCreate(linkCoords, link.facilityID);
        if (!success) continue;

        // We registers the linked facility as a new link
        auto pFacility = facilitiesFind(linkCoords, link.facilityID);
        pFacility->stronglyLinked = true;
        facilityLinksAdd(facility, &link, linkCoords);
    }
}

void Data::roomLinksStrongRemoveFacilities(const RoomCoords& coords)
{
    auto& roomInfo = room(coords);
    for (auto& facility : roomInfo.facilities)
        facilityLinksStrongRemoveFacilities(facility);
}

void Data::roomLinksStrongRecreateFacilities(const RoomCoords& coords)
{
    auto& roomInfo = room(coords);
    auto facilities = roomInfo.facilities;
    for (auto& facility : facilities)
        facilityLinksStrongRecreateFacilities(facility);
}

void Data::roomLinksIncomingStrongRemoveFacilities(const RoomCoords& coords)
{
    auto& roomInfo = room(coords);
    auto facilities = roomInfo.facilities;
    for (auto& facility : facilities)
        if (facility.stronglyLinked)
            facilitiesRemove(facility.coords, facility.data.type(), true);
}

void Data::roomLinksIncomingRemove(const RoomCoords& coords)
{
    for (auto& floor : m_floors)
    for (auto& room : floor.rooms)
    for (auto& facility : room.facilities) {
        auto& links = facility.links;
        auto newEnd = std::remove_if(std::begin(links), std::end(links), [&coords] (const FacilityLink& link) { return link.coords == coords; });
        if (newEnd != std::end(facility.links)) {
            facility.links.erase(newEnd, std::end(facility.links));
            addEvent("facility_changed", facility.coords);
        }
    }
}

void Data::roomLinksBreakableRemove(const RoomCoords& coords)
{
    bool facilitiesChanged = false;

    auto& roomInfo = room(coords);
    for (auto& facility : roomInfo.facilities) {
        auto& links = facility.links;
        auto newEnd = std::remove_if(std::begin(links), std::end(links), [] (const FacilityLink& link) { return link.common != nullptr && !link.common->unbreakable; });
        facilitiesChanged = (newEnd != std::end(links));
        if (!facilitiesChanged) continue;
        facility.links.erase(newEnd, std::end(links));
    }

    if (facilitiesChanged)
        addEvent("facility_changed", coords);
}

//----- Barrier

void Data::setRoomFacilityBarrier(const RoomCoords& coords, const std::wstring& facilityID, bool activated)
{
    auto pFacilityInfo = facilitiesFind(coords, facilityID);
    returnif (pFacilityInfo == nullptr);

    pFacilityInfo->barrier = activated;
    addEvent("facility_changed", coords);
    EventEmitter::addEvent("dungeon_changed");
}

//----- Treasure

void Data::setRoomFacilityTreasure(const RoomCoords& coords, const std::wstring& facilityID, uint32 amount)
{
    auto pFacilityInfo = facilitiesFind(coords, facilityID);
    returnif (pFacilityInfo == nullptr);

    pFacilityInfo->treasure = amount;
    addEvent("treasure_changed", coords);
}

//----- Tunnels

void Data::addFacilityTunnel(FacilityInfo& facilityInfo, const sf::Vector2i& tunnelCoords, bool relative)
{
    Tunnel tunnel;
    tunnel.coords = tunnelCoords;
    tunnel.relative = relative;
    facilityInfo.tunnels.emplace_back(std::move(tunnel));
    EventEmitter::addEvent("dungeon_changed");
}

//-----------------//
//----- Traps -----//

bool Data::trapIs(const RoomCoords& coords, const std::wstring& trapID) const
{
    returnif (!isRoomConstructed(coords)) false;
    auto& trapData = room(coords).trap.data;
    return (trapData.exists() && trapData.type() == trapID);
}

bool Data::trapSetValid(const RoomCoords& coords, const std::wstring& trapID) const
{
    returnif (trapIs(coords, trapID)) false;

    // Note: The previous trap does not block construction as it is going to be replaced
    //       by the new one. However, we prevent changing traps if they are of the same type.

    // Check lock validity
    const auto& trapData = trapsDB().get(trapID);
    auto trapPrint = trapData.lock & roomLock(coords, false, false);
    returnif (trapPrint != 0_u8) false;

    // TODO Check againt absolute constraints for this trap
    // Well, when traps will have constraints
    return true;
}

void Data::setRoomTrap(const RoomCoords& coords, const std::wstring& trapID)
{
    returnif (!trapSetValid(coords, trapID));
    auto& trapInfo = room(coords).trap;

    // Note: This event needs to be before the permissive removals,
    //       so that it is correctly queued for Inter.
    addEvent("trap_changed", coords);

    // Remove permissive facilities that are in the way
    auto& trapData = m_trapsDB.get(trapID);
    facilitiesPermissiveRemoveLocking(coords, trapData.lock);

    // Destroy previous trap if any
    trapInfo.data.clear();
    trapInfo.barrier = false;

    // And and set it to the new one
    trapInfo.data.create(trapID);
    trapInfo.common = &m_trapsDB.get(trapID);

    // Changing the trap, some strongly linked facilities might be able to be recreated
    roomLinksIncomingStrongRecreateFacilities(coords);
}

void Data::removeRoomTrap(const RoomCoords& coords)
{
    returnif (!isRoomConstructed(coords));

    auto& roomInfo = room(coords);
    returnif (!roomInfo.trap.data.exists());
    roomInfo.trap.data.clear();

    // Some strongly linked facilities might be able to be recreated
    roomLinksIncomingStrongRecreateFacilities(coords);

    addEvent("trap_changed", coords);
}

void Data::setRoomTrapBarrier(const RoomCoords& coords, bool activated)
{
    auto& roomInfo = room(coords);
    returnif (!roomInfo.trap.data.exists());

    roomInfo.trap.barrier = activated;
    addEvent("trap_changed", coords);
    EventEmitter::addEvent("dungeon_changed");
}

void Data::setTrapsGenericUnlocked(bool unlocked)
{
    for (auto& trapInfoPair : m_trapsGenerics)
        trapInfoPair.second.unlocked = unlocked;
    EventEmitter::addEvent("trap_generic_changed");
}

void Data::setTrapGenericUnlocked(const std::wstring& trapID, bool unlocked)
{
    m_trapsGenerics[trapID].unlocked = unlocked;
    EventEmitter::addEvent("trap_generic_changed");
}

//--------------------//
//----- Monsters -----//

void Data::removeRoomMonsters(const RoomCoords& coords)
{
    m_monstersManager.removeRoomMonsters(coords);
}

bool Data::addMonsterValid(const RoomCoords& coords, const std::wstring& monsterID)
{
    // Note: We do not care about monsterID.
    // But, one day, we might.
    // For instance, it might be impossible to place a monster directly on some trap.

    return isRoomWalkable(coords);
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

void Data::moveMonsterFromReserve(const RoomCoords& coords, const std::wstring& monsterID)
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

void Data::setMonstersGenericUnlocked(bool unlocked)
{
    for (auto& monsterInfoPair : m_monstersGenerics)
        monsterInfoPair.second.unlocked = unlocked;
    EventEmitter::addEvent("monster_generic_changed");
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

void Data::changedFloorRoomsCount()
{
    correctFloorsRooms();
}
