#pragma once

#include "tools/param.hpp"
#include "tools/int.hpp"

#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>

namespace dungeon
{
    class Data
    {
    public:

        //-----------------//
        //----- Enums -----//

        //! Defines rooms state.
        enum class RoomState {
            UNKNOWN,
            VOID,
            CONSTRUCTED,
        };

        //-----------------//
        //----- Types -----//

        //! A room as in the xml specification.
        struct Room {
            uint floorPos;
            uint pos;
            RoomState state = RoomState::UNKNOWN;

            struct Facilities {
                bool ladder = false;
            } facilities;
        };

        //! A floor is a vector of rooms.
        struct Floor {
            uint pos;
            std::vector<Room> rooms;
        };

    public:

        Data() = default;
        virtual ~Data() = default;

        //-------------------------//
        //----- Import/Export -----//

        //! Load dungeon data from a file.
        void load(const std::string& file);

        //! Save dungeon data to a file.
        void save(const std::string& file);

        //-------------------//
        //----- Getters -----//

        //! Easy getter to access floors.
        std::vector<Floor>& floors() { return m_floors; }

        //! Easy getter to access a room.
        Room& room(const sf::Vector2u& floorRoom) { return m_floors[floorRoom.x].rooms[floorRoom.y]; }

    protected:

        //-------------------//
        //----- Changes -----//

        //! Callback on \ref floorCount property.
        void changedFloorsCount();

        //! Callback on \ref roomsByFloor property.
        void changedRoomsByFloor();

        //-----------------------//
        //----- Corrections -----//

        //! Correct the dungeon data.
        /*!
         *  Will resize the room/floor vectors to current values
         *  held in floorsCount and roomsByFloor properties,
         *  and replace all RoomState::UNKNOWN to RoomState::VOID.
         */
        void correctFloorsRooms();

        //------------------//
        //----- Params -----//

        //! Name of the dungeon.
        //! @access name
        PARAMG(std::wstring, m_name, name)

        //! Number of floors in the dungeon.
        //! @access floorsCount setFloorsCount changedFloorsCount
        PARAMGSU(uint, m_floorsCount, floorsCount, setFloorsCount, changedFloorsCount)

        //! Number of rooms in each floor.
        //! @access roomsByFloor setRoomsByFloor changedRoomsByFloor
        PARAMGSU(uint, m_roomsByFloor, roomsByFloor, setRoomsByFloor, changedRoomsByFloor)

    private:

        //! A dungeon consists in a vector of floors.
        std::vector<Floor> m_floors;
    };
}
