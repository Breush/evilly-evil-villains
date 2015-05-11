#pragma once

#include "tools/param.hpp"
#include "tools/int.hpp"
#include "dungeon/event.hpp"
#include "dungeon/eventtype.hpp"
#include "dungeon/facilities.hpp"
#include "dungeon/trapdata.hpp"

#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>
#include <array>

// Forward declarations

namespace pugi
{
    class xml_node;
}

namespace dungeon
{
    // Forward declarations

    class Graph;

    //! The data of a dungeon.
    /*!
     *  Can import and export dungeon and resources data to file.
     */

    class Data final : public EventEmitter
    {
        friend class Graph;

    public:

        //! Defines the possibly accessible relative directions from a room.
        /*!
         *  They are defined this way to be quickly masked.
         *  So that (direction & 0xf - 1u) gives WEST -1 and EAST +1 (x axis)
         *  And (direction >> 0x4 - 1u) gives SOUTH -1 and NORTH +1 (y axis)
         */
        enum Direction : uint8
        {
            EAST =  0x12,
            WEST =  0x10,
            NORTH = 0x21,
            SOUTH = 0x01,
        };

        //! Defines rooms state.
        enum class RoomState
        {
            UNKNOWN,        //!< Error state.
            VOID,           //!< Empty.
            CONSTRUCTED,    //!< Constructed.
        };

        //! A room as in the xml specification.
        struct Room
        {
            sf::Vector2u coords;                            //!< The floor/room coordinate of the room.
            RoomState state = RoomState::UNKNOWN;           //!< The current state.

            // TODO Use the same trick than for trap
            std::array<bool, FacilityID::COUNT> facilities; //!< All the facilities.
            uint treasureDosh = 0u;                         //!< If facility treasure, then the stored dosh.

            TrapData trap;                                  //!< The trap protecting the room.
        };

        //! A floor is a vector of rooms.
        struct Floor
        {
            uint pos;
            std::vector<Room> rooms;
        };

    public:

        //! Constructor.
        Data();

        //! Default destructor.
        ~Data() = default;

        //------------------------//
        //! @name File management
        //! @{

        //! Load data from a specified folder (must exists).
        //! @return The filename of the main dungeon file.
        std::wstring load(const std::wstring& folder);

        //! Save data to a specified folder (must exists).
        //! @return The filename of the main dungeon file.
        std::wstring save(const std::wstring& folder);

        //! Save data to a specified folder (must exists).
        void createFiles(const std::wstring& folder);

        //! @}

        //--------------//
        //! @name Rooms
        //! @{

        //! Easy getter to access a room.
        inline Room& room(const sf::Vector2u& coords) { return m_floors[coords.x].rooms[coords.y]; }

        //! Whether a specific is in constructed state.
        bool isRoomConstructed(const sf::Vector2u& coords);

        //! Construct a room.
        void constructRoom(const sf::Vector2u& coords);

        //! Destroy a room.
        void destroyRoom(const sf::Vector2u& coords);

        //! Return true if you can access the next room from the specified one.
        //! In the case of unexisting room, it returns false.
        bool roomNeighbourAccessible(const sf::Vector2u& roomCoord, Direction direction);

        //! Return the coordinates of the next room from the specified one.
        /*!
         *  Note: this function does not check accessibility.
         *  The results might be beyond dungeon boundaries.
         */
        sf::Vector2u roomNeighbourCoords(const sf::Vector2u& roomCoord, Direction direction);

        //! @}

        //-----------------------------//
        //! @name Facilities and traps
        //! @{

        //! Set the value of the specified room's facility.
        //! Will emit an event if a change occured.
        void setRoomFacility(const sf::Vector2u& coords, FacilityID facilityID, bool state);

        //! Set the trap of the specified room.
        //! Will emit an event if a change occured.
        void setRoomTrap(const sf::Vector2u& coords, const std::wstring& trapID);

        //! @}

        //-------------------------//
        //! @name Hero interaction
        //! @{

        //! When a hero steals some dosh from the treasure room.
        void stealTreasure(const sf::Vector2u& coords, Hero& hero, uint stolenDosh);

        //! @}

        //------------------//
        //! @name Resources
        //! @{

        inline uint dosh() const { return m_dosh; } //!< Get dosh value;
        inline uint fame() const { return m_fame; } //!< Get fame value;

        //! Add dosh to current value.
        void addDosh(uint dosh);

        //! Remove dosh from current value.
        void subDosh(uint dosh);

        //! @}

        //-------------//
        //! @name Mode
        //! @{

        inline Mode mode() const { return m_mode; } //!< Get the current mode.

        //! Set the current mode.
        void setMode(Mode mode);

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! Name of the dungeon.
        //! @access name setName
        PARAMGS(std::wstring, m_name, name, setName)

        //! Number of floors in the dungeon.
        //! @access floorsCount setFloorsCount changedFloorsCount
        PARAMGSU(uint, m_floorsCount, floorsCount, setFloorsCount, changedFloorsCount)

        //! Number of rooms in each floor.
        //! @access roomsByFloor setRoomsByFloor changedRoomsByFloor
        PARAMGSU(uint, m_roomsByFloor, roomsByFloor, setRoomsByFloor, changedRoomsByFloor)

        //! @}

    protected:

        //----------------------//
        //! @name Event emitter
        //! @{

        //! Emits a default event type through all receivers.
        void emit(EventType eventType) final;

        //! @}

        //---------------------//
        //! @name Dungeon data
        //! @{

        //! Load dungeon data from a specified file (must exists).
        void loadDungeon(const std::wstring& file);

        //! Save dungeon data to a specified file (must exists).
        void saveDungeon(const std::wstring& file);

        //! Helpers loading and saving  facilities.
        void loadDungeonRoomFacilities(Room& room, const pugi::xml_node& node);
        void saveDungeonRoomFacilities(const Room& room, pugi::xml_node& node);

        //! @}

        //--------------//
        //! @name Rooms
        //! @{

        //! Return the next room from the specified one.
        /*!
         *  Note: this function does not check accessibility.
         *  The next room should not be out of the limits of the dungeon.
         */
        Room& roomNeighbour(const sf::Vector2u& roomCoord, Direction direction);

        //! Returns a unit vector symbolizing the direction.
        sf::Vector2u roomDirectionVector(Direction direction);

        //! @}

        //----------------------//
        //! @name Graph linking
        //! @{

        //! Set the graph to be linked with these data.
        void linkGraph(Graph* graph) { m_graph = graph; }

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Callback on floorCount property.
        void changedFloorsCount();

        //! Callback on roomsByFloor property.
        void changedRoomsByFloor();

        //! Correct the dungeon data.
        /*!
         *  Will resize the room/floor vectors to current values
         *  held in floorsCount and roomsByFloor properties,
         *  and replace all RoomState::UNKNOWN to RoomState::VOID.
         */
        void correctFloorsRooms();

        //! @}

    private:

        //! A dungeon consists in a vector of floors.
        std::vector<Floor> m_floors;

        Graph* m_graph = nullptr;   //!< The graph linked to these data.

        Mode m_mode = Mode::DESIGN; //!< The current mode.

        uint m_dosh = 0u;   //!< The resource dosh value.
        uint m_fame = 0u;   //!< The resource fame value.
    };
}

