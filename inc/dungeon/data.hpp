#pragma once

#include "tools/param.hpp"

#include <string>
#include <vector>

namespace dungeon
{
    class Data
    {
    public:
        // Defines rooms state
        enum RoomState {
            UNKNOWN,
            VOID,
            CONSTRUCTED,
        };

        // A room as in the xml spec'
        struct Floor;
        struct Room {
            Floor& floor;
            uint pos;
            RoomState state;
        };

        // Floor info
        struct Floor {
            uint pos;
            std::vector<Room> rooms;
        };

    public:
        Data();
        virtual ~Data() {}

        // Import
        void load(const std::string& file);

        // Getters
        std::vector<Floor>& floors() { return m_floors; }

    protected:
        // Changes
        void changedFloorsCount();
        void changedRoomsByFloor();

        // Params
        PARAMG(std::string, m_name, name);
        PARAMGSU(uint, m_floorsCount, floorsCount, setFloorsCount, changedFloorsCount)
        PARAMGSU(uint, m_roomsByFloor, roomsByFloor, setRoomsByFloor, changedRoomsByFloor)

    private:
        std::vector<Floor> m_floors;
    };
}
