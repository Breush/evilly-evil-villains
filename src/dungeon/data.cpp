#include "dungeon/data.hpp"

#include "tools/debug.hpp"

#include <pugixml.hpp>
#include <stdexcept>

using namespace dungeon;

Data::Data()
{
}

void Data::load(const std::string& file)
{
    m_floors.clear();

    // Parsing XML
    pugi::xml_document doc;
    doc.load_file(file.c_str());

    const auto& dungeon = doc.child("dungeon");
    if (!dungeon) throw std::runtime_error("File " + file + " is not a valid dungeon file.");

    // Dungeon
    m_name = dungeon.attribute("name").as_string();
    m_floorsCount = dungeon.attribute("floorsCount").as_uint();
    m_roomsByFloor = dungeon.attribute("roomsByFloor").as_uint();

    mdebug_dungeon_1("Loading file " << file);
    mdebug_dungeon_1("Dungeon is " << m_name << " of size " << m_floorsCount << "x" << m_roomsByFloor);

    // Floors
    m_floors.reserve(m_floorsCount);
    for (auto& floor : dungeon.children("floor")) {
        auto floorPos = floor.attribute("pos").as_uint();

        m_floors.push_back(Floor{floorPos});
        mdebug_dungeon_2("Found floor " << floorPos);

        // Rooms
        m_floors[floorPos].rooms.reserve(m_roomsByFloor);
        for (auto& room : floor.children("room")) {
            auto roomPos = room.attribute("pos").as_uint();
            std::string roomStateString = room.attribute("state").as_string();

            RoomState roomState(RoomState::UNKNOWN);
            if (roomStateString == "void") roomState = RoomState::VOID;
            else if (roomStateString == "constructed") roomState = RoomState::CONSTRUCTED;

            m_floors[floorPos].rooms.push_back(Room{m_floors[floorPos], roomPos, roomState});
            mdebug_dungeon_3("Found room " << roomPos << " of state " << roomStateString);
        }
    }
}

//-------------------//
//----- Changes -----//

void Data::changedFloorsCount()
{
}

void Data::changedRoomsByFloor()
{
}
