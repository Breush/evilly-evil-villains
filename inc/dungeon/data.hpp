#pragma once

#include "tools/param.hpp"
#include "tools/int.hpp"
#include "dungeon/event.hpp"
#include "dungeon/elementdata.hpp"
#include "dungeon/databases/monstersdb.hpp"
#include "dungeon/databases/trapsdb.hpp"
#include "dungeon/databases/facilitiesdb.hpp"
#include "context/wallet.hpp"

#include <SFML/System/Time.hpp>

#include <string>
#include <vector>
#include <array>

// Forward declarations

namespace pugi
{
    class xml_node;
}

namespace context
{
    class Villain;
}

namespace dungeon
{
    // TODO Move these to their own file
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

    //! A facility information.
    struct FacilityInfo
    {
        ElementData data;               //!< The data.
        std::vector<Direction> rlinks;  //!< Relative links that allow a specific direction.
        bool isLink = false;            //!< Is these info been created by a link?
    };

    // Forward declarations

    class Graph;

    //! The data of a dungeon.
    /*!
     *  Can import and export dungeon and resources data to file.
     */

    class Data final : public context::EventEmitter
    {
        friend class Graph;

    public:

        const uint onConstructRoomCost = 1100u; //!< The dosh cost for creating a room.
        const uint onDestroyRoomGain = 745u;    //!< The dosh gain when destroying a room.

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
            sf::Vector2u coords;                    //!< The floor/room coordinates of the room.
            RoomState state = RoomState::UNKNOWN;   //!< The current state.

            std::vector<FacilityInfo> facilities;   //!< All the facilities.
            ElementData trap;                       //!< The trap protecting the room.
        };

        //! A floor is a vector of rooms.
        struct Floor
        {
            uint pos;
            std::vector<Room> rooms;
        };

        //! A monster as stored in the xml.
        struct MonsterInfo
        {
            ElementData data;       //!< Type of monster, plus individual information.
        };

        //! A reserve slot about a monster.
        struct MonsterCageInfo
        {
            std::wstring type;                  //!< Type of monster.
            uint countdown = 0u;                //!< How many seconds left before reactivation of Hire button.
            std::vector<MonsterInfo> monsters;  //!< The monster in the reserve.
        };

        //! Holding monsters data.
        struct MonstersInfo
        {
            std::vector<MonsterCageInfo> reserve;   //!< All the cages.
            std::vector<MonsterInfo> active;        //!< The active monsters (in dungeon).
        };

    public:

        //! Constructor.
        Data();

        //! Default destructor.
        ~Data() = default;

        //----------------//
        //! @name Routine
        //! @{

        //! Routine call for time update.
        void update(const sf::Time& dt);

        //! @}

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
        //! If coords are outside of boundaries, returns false.
        bool isRoomConstructed(const sf::Vector2u& coords);

        //! Construct a room.
        //! @param hard Do not check for money.
        void constructRoom(const sf::Vector2u& coords, bool hard = false);

        //! Destroy a room.
        //! @param hard Destroy monsters inside.
        void destroyRoom(const sf::Vector2u& coords);

        //! Return true if you can access the next room from the specified one.
        //! In the case of unexisting room, it returns false.
        bool roomNeighbourAccessible(const sf::Vector2u& coords, Direction direction);

        //! Return the coordinates of the next room from the specified one.
        /*!
         *  Note: this function does not check accessibility.
         *  The results might be beyond dungeon boundaries.
         */
        sf::Vector2u roomNeighbourCoords(const sf::Vector2u& coords, Direction direction);

        //! Gets the dosh contained in the treasure facility if any (returns 0u if no treasure facility).
        uint roomTreasureDosh(const sf::Vector2u& coords);

        //! @}

        //-----------------------------//
        //! @name Facilities and traps
        //! @{

        //! Add the specified facility to the dungeon if it does not exists yet.
        //! Will emit an event if a change occured.
        void createRoomFacility(const sf::Vector2u& coords, const std::wstring& facilityID, bool isLink = false);

        //! Remove the specified facility from the dungeon.
        //! Will emit an event if a change occured.
        void removeRoomFacility(const sf::Vector2u& coords, const std::wstring& facilityID);

        //! Remove all the facilities from the room specified.
        //! Will emit an event if a change occured.
        void removeRoomFacilities(const sf::Vector2u& coords);

        //! Set the trap of the specified room.
        //! Will emit an event if a change occured.
        void setRoomTrap(const sf::Vector2u& coords, const std::wstring& trapID);

        //! Remove the trap of the specified room (if any).
        //! Will emit an event if a change occured.
        void removeRoomTrap(const sf::Vector2u& coords);

        //! Is adding the monster a valid action? Checks against dungeon consistency.
        bool addMonsterValid(const sf::Vector2u& coords, const std::wstring& monsterID);

        //! Add the specified type of monster to the reserve.
        void addMonsterToReserve(const std::wstring& monsterID, const uint countdownIncrease = 0u);

        //! Move the specified type of monster from reserve into the dungeon.
        void moveMonsterFromReserve(const sf::Vector2u& coords, const std::wstring& monsterID);

        //! Remove all the monsters from the room specified.
        //! Will emit an event if a change occured.
        void removeRoomMonsters(const sf::Vector2u& coords);

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

        inline uint fame() const { return m_fameWallet.value(); }           //!< Get fame value.
        inline void setFame(uint value) { m_fameWallet.set(value); }        //!< Set the fame to a specific value.
        inline bool addFame(uint value) { return m_fameWallet.add(value); } //! Add fame to current value.
        inline bool subFame(uint value) { return m_fameWallet.sub(value); } //! Substract fame from current value.

        inline uint time() const { return m_time; } //!< Access the time.

        //! @}

        //----------------//
        //! @name Getters
        //! @{

        //! Access the list of all monsters inside dungeon and reserve.
        inline MonstersInfo& monstersInfo() { return m_monstersInfo; }

        //! Acces the facilities data base.
        inline const FacilitiesDB& facilitiesDB() const { return m_facilitiesDB; }

        //! Acces the monsters data base.
        inline const MonstersDB& monstersDB() const { return m_monstersDB; }

        //! Acces the traps data base.
        inline const TrapsDB& trapsDB() const { return m_trapsDB; }

        //! Access the associated villain.
        inline context::Villain& villain() { return *m_villain; }

        //! Access the associated graph.
        inline Graph& graph() { return *m_graph; }

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

        //---------------------//
        //! @name Dungeon data
        //! @{

        //! Load dungeon data from a specified file (must exists).
        void loadDungeon(const std::wstring& file);

        //! Save dungeon data to a specified file (must exists).
        void saveDungeon(const std::wstring& file);

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

        Graph* m_graph = nullptr;   //!< The graph linked to these data.
        context::Villain* m_villain = nullptr;  //!< The villain reference.
        context::Wallet m_fameWallet;           //!< The resource fame value.

        std::vector<Floor> m_floors;    //!< A dungeon consists in a vector of floors.

        // Monsters
        MonstersInfo m_monstersInfo;    //!< A dungeon also has some list of monsters.

        // Time
        uint m_time = 0u;           //!< How much time the dungeon has been constructed, in in-game hours.
        const float m_timeGameHour; //!< Constant time: howmany real seconds equals an in-game hour.

        // Databases
        MonstersDB m_monstersDB;        //!< All monsters immuable data.
        TrapsDB m_trapsDB;              //!< All traps immuable data.
        FacilitiesDB m_facilitiesDB;    //!< All facilities immuable data.
    };
}

