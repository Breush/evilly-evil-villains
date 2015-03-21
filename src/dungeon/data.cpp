#include "dungeon/data.hpp"

#include "tools/debug.hpp"
#include "tools/string.hpp"

#include <pugixml.hpp>
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

    loadDungeon(folder + L"dungeon.xml");
}

void Data::save(const std::wstring& folder)
{
    #if DEBUG_GLOBAL > 0
        saveDungeon(folder + L"dungeon_saved.xml");
    #else
        saveDungeon(folder + L"dungeon.xml");
    #endif

    wdebug_dungeon_1(L"Saved data to folder " << folder);
}

void Data::createFiles(const std::wstring& folder)
{
    saveDungeon(folder + L"dungeon.xml");

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
        throw std::runtime_error("File " + toString(file) + " is not a valid dungeon file.");

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

                if (type == L"ladder") room.facilities.ladder = true;

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
