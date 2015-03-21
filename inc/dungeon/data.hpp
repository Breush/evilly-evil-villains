#pragma once

#include "tools/param.hpp"
#include "tools/int.hpp"

#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>

namespace dungeon
{
    class Data final
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

        Data();
        ~Data() = default;

        //-------------------------//
        //----- Import/Export -----//

        //! Load data from a specified folder (must exists).
        void load(const std::wstring& folder);

        //! Save data to a specified folder (must exists).
        void save(const std::wstring& folder);

        //! Save data to a specified folder (must exists).
        void createFiles(const std::wstring& folder);

        //-------------------//
        //----- Getters -----//

        //! Easy getter to access floors.
        inline std::vector<Floor>& floors() { return m_floors; }

        //! Easy getter to access a room.
        inline Room& room(const sf::Vector2u& floorRoom) { return m_floors[floorRoom.x].rooms[floorRoom.y]; }

        inline uint dosh() const { return m_dosh; }
        inline uint fame() const { return m_fame; }

    protected:

        //---------------------//
        //! @name Dungeon data
        //! @{

        //! Load dungeon data from a specified file (must exists).
        void loadDungeon(const std::wstring& file);

        //! Save dungeon data to a specified file (must exists).
        void saveDungeon(const std::wstring& file);

        //! @}

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
        //! @access name setName
        PARAMGS(std::wstring, m_name, name, setName)

        //! Number of floors in the dungeon.
        //! @access floorsCount setFloorsCount changedFloorsCount
        PARAMGSU(uint, m_floorsCount, floorsCount, setFloorsCount, changedFloorsCount)

        //! Number of rooms in each floor.
        //! @access roomsByFloor setRoomsByFloor changedRoomsByFloor
        PARAMGSU(uint, m_roomsByFloor, roomsByFloor, setRoomsByFloor, changedRoomsByFloor)

    private:

        //! A dungeon consists in a vector of floors.
        std::vector<Floor> m_floors;

        uint m_dosh = 0u;   //!< The resource dosh value.
        uint m_fame = 0u;   //!< The resource fame value.
    };
}
