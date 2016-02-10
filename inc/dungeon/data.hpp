#pragma once

#include "tools/param.hpp"
#include "tools/int.hpp"
#include "dungeon/event.hpp"
#include "dungeon/managers/heroesmanager.hpp"
#include "dungeon/managers/monstersmanager.hpp"
#include "dungeon/managers/dynamicsmanager.hpp"
#include "dungeon/elements/elementdata.hpp"
#include "dungeon/databases/facilitiesdb.hpp"
#include "dungeon/databases/monstersdb.hpp"
#include "dungeon/databases/heroesdb.hpp"
#include "dungeon/databases/trapsdb.hpp"
#include "dungeon/structs/direction.hpp"
#include "dungeon/structs/monster.hpp"
#include "dungeon/structs/room.hpp"
#include "context/villains.hpp"

#include <SFML/System/Time.hpp>

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

    class Data final : public context::EventEmitter
    {
        friend class Graph;

    public:

        const uint onConstructRoomCost = 1100u; //!< The dosh cost for creating a room.
        const uint onDestroyRoomGain = 745u;    //!< The dosh gain when destroying a room.

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

        //----------------//
        //! @name Emitter
        //! @{

        //! Quick tool to emit an dungeon event with coords informations.
        void addEvent(std::string eventType, const RoomCoords& coords);

        //! @}

        //--------------//
        //! @name Rooms
        //! @{

        //! Easy getter to access a room.
        inline Room& room(const RoomCoords& coords) { return m_floors[coords.x].rooms[coords.y]; }

        //! Easy getter to access a room (const).
        inline const Room& room(const RoomCoords& coords) const { return m_floors[coords.x].rooms[coords.y]; }

        //! Return the next room from the specified one.
        /*!
         *  Note: this function does not check accessibility.
         *  The next room should not be out of the limits of the dungeon.
         */
        Room& roomNeighbour(const RoomCoords& roomCoord, Direction direction);

        //! Returns a unit vector symbolizing the direction (int).
        RoomDirection roomDirectionVector2i(Direction direction);

        //! Returns a unit vector symbolizing the direction (uint).
        RoomCoords roomDirectionVector2u(Direction direction);

        //! Whether a specific room is in constructed state.
        //! If coords are outside of boundaries, returns false.
        bool isRoomConstructed(const RoomCoords& coords) const;

        //! Whether a specific room allows walking.
        bool isRoomWalkable(const RoomCoords& coords) const;

        //! Construct a room.
        void constructRoom(const RoomCoords& coords);

        //! Construct all rooms, in memory, no events launched.
        void constructRoomsAll();

        //! Destroy a room.
        void destroyRoom(const RoomCoords& coords);

        //! Push the room at coords if any.
        //! @return true on success or if no room and false if action is impossible.
        bool pushRoom(const RoomCoords& coords, Direction direction);

        //! Remove some dosh from the room.
        //! Returns the amount stolen, between 0u and wantedDosh;
        uint stealRoomTreasure(const RoomCoords& coords, uint wantedDosh);

        //! Return the coordinates of the next room from the specified one.
        /*!
         *  Note: this function does not check accessibility.
         *  The results might be beyond dungeon boundaries.
         */
        RoomCoords roomNeighbourCoords(const RoomCoords& coords, Direction direction);

        //! Gets the dosh contained in the treasure facility if any (returns 0u if no treasure facility).
        uint roomTreasureDosh(const RoomCoords& coords);

        //! @}

        //-------------------//
        //! @name Facilities
        //! @{

        //! Is creating the facility a valid action? Checks against dungeon consistency.
        bool createRoomFacilityValid(const RoomCoords& coords, const std::wstring& facilityID);

        //! Returns true if a facility exists in the specified coordinates.
        bool hasFacility(const RoomCoords& coords, const std::wstring& facilityID) const;

        //! Quick access to facility info.
        //! Returns nullptr if not found or invalid coordinates.
        FacilityInfo* facilitiesFind(const RoomCoords& coords, const std::wstring& facilityID);

        //! Quick access to facility info (const).
        //! Returns nullptr if not found or invalid coordinates.
        const FacilityInfo* facilitiesFind(const RoomCoords& coords, const std::wstring& facilityID) const;

        //! Add the specified facility to the dungeon if it does not exists yet, if mark it as a link.
        //! Will emit an event if a change occured.
        bool facilitiesCreate(const RoomCoords& coords, const std::wstring& facilityID, uint fixedLinkID);

        //! Add the specified facility to the dungeon if it does not exists yet.
        //! Will emit an event if a change occured.
        bool facilitiesCreate(const RoomCoords& coords, const std::wstring& facilityID);

        //! Remove the specified facility from the dungeon.
        //! This will also remove all strong links the facility had.
        //! Will emit an event if a change occured.
        void facilitiesRemove(const RoomCoords& coords, const std::wstring& facilityID);

        //! Remove all the facilities from the room specified.
        //! Will emit an event if a change occured.
        void facilitiesRemove(const RoomCoords& coords);

        //----- Links

        //! Add a link to a facility.
        void facilityLinksAdd(const RoomCoords& coords, const std::wstring& facilityID, const Link* common, const RoomCoords& linkCoords, bool relink = false);

        //! Add a link to a facility.
        void facilityLinksAdd(FacilityInfo& facilityInfo, const Link* common, const RoomCoords& linkCoords, bool relink = false);

        //! Remove a specific link of a facility.
        void facilityLinksRemove(const RoomCoords& coords, const std::wstring& facilityID, const RoomCoords& linkCoords, const std::wstring& linkFacilityID);

        //! Remove all strongly linked facilities.
        void facilityLinksStrongRemoveFacilities(FacilityInfo& facilityInfo);

        //! Recreate all strongly linked facilities.
        void facilityLinksStrongRecreateFacilities(FacilityInfo& facility);

        //! Remove all links pointing to the facility (using the relink).
        void facilityLinksIncomingRemove(const RoomCoords& coords, const std::wstring& facilityID);

        //! Create all linked facilities that should exist in this room.
        void roomLinksIncomingStrongRecreateFacilities(const RoomCoords& coords);

        //! Remove all strongly linked facilities of a room.
        void roomLinksStrongRemoveFacilities(const RoomCoords& coords);

        //! Recreate all strongly linked facilities of a room.
        void roomLinksStrongRecreateFacilities(const RoomCoords& coords);

        //! Remove all strongly facilities with the room.
        void roomLinksIncomingStrongRemoveFacilities(const RoomCoords& coords);

        //! Remove all incoming links of a room.
        void roomLinksIncomingRemove(const RoomCoords& coords);

        //! Remove all breakable links of a room.
        //! For instance, when pushing a room with a sensor, its link won't be broken.
        //! But if the room contains stairs, it will.
        void roomLinksBreakableRemove(const RoomCoords& coords);

        //----- Barrier

        //! Set the specified room facility's barrier.
        void setRoomFacilityBarrier(const RoomCoords& coords, const std::wstring& facilityID, bool activated);

        //----- Treasure

        //! Set the specified room facility's treasure.
        void setRoomFacilityTreasure(const RoomCoords& coords, const std::wstring& facilityID, uint32 amount);

        //----- Tunnels

        //! Add a tunnel to the specificed room facility.
        void addFacilityTunnel(FacilityInfo& facilityInfo, const sf::Vector2i& tunnelCoords, bool relative);

        //! @}

        //--------------//
        //! @name Traps
        //! @{

        //! Set the trap of the specified room.
        //! Will emit an event if a change occured.
        void setRoomTrap(const RoomCoords& coords, const std::wstring& trapID);

        //! Remove the trap of the specified room (if any).
        //! Will emit an event if a change occured.
        void removeRoomTrap(const RoomCoords& coords);

        //! Set the specified room trap's barrier.
        void setRoomTrapBarrier(const RoomCoords& coords, bool activated);

        //! Set all traps generic unlocked state.
        void setTrapsGenericUnlocked(bool unlocked);

        //! Set the trap generic unlocked state.
        void setTrapGenericUnlocked(const std::wstring& trapID, bool unlocked);

        //! @}

        //-----------------//
        //! @name Monsters
        //! @{

        //! Is adding the monster a valid action? Checks against dungeon consistency.
        bool addMonsterValid(const RoomCoords& coords, const std::wstring& monsterID);

        //! Add the specified type of monster to the reserve.
        void addMonsterToReserve(const std::wstring& monsterID, const uint countdownIncrease = 0u);

        //! Move the specified type of monster from reserve into the dungeon.
        void moveMonsterFromReserve(const RoomCoords& coords, const std::wstring& monsterID);

        //! Remove all the monsters from the room specified.
        //! Will emit an event if a change occured.
        void removeRoomMonsters(const RoomCoords& coords);

        //! Set all traps generic unlocked state.
        void setMonstersGenericUnlocked(bool unlocked);

        //! Set the monster generic unlocked state.
        void setMonsterGenericUnlocked(const std::wstring& monsterID, bool unlocked);

        //! @}

        //------------------//
        //! @name Resources
        //! @{

        //! Set a debt that will be payed weekly.
        void setDebt(uint debtPerWeek, uint debtWeeksDuration);

        //! Access to the villain's dosh wallet.
        inline context::Wallet& doshWallet() { return m_villain->doshWallet; }

        //! Access to the villain's dosh wallet (const).
        inline const context::Wallet& doshWallet() const { return m_villain->doshWallet; }

        //! Access to the fame wallet.
        inline context::Wallet& fameWallet() { return m_fameWallet; }

        //! Access to the fame wallet (const).
        inline const context::Wallet& fameWallet() const { return m_fameWallet; }

        //! Access to the soul wallet.
        inline context::Wallet& soulWallet() { return m_soulWallet; }

        //! Access to the soul wallet (const).
        inline const context::Wallet& soulWallet() const { return m_soulWallet; }

        //!< Access to the current game time.
        inline uint time() const { return m_time; }

        //! @}

        //----------------//
        //! @name Getters
        //! @{

        //! Access the facilities data base.
        inline const FacilitiesDB& facilitiesDB() const { return m_facilitiesDB; }

        //! Access the monsters data base.
        inline const MonstersDB& monstersDB() const { return m_monstersDB; }

        //! Access the monsters generics.
        inline const std::unordered_map<std::wstring, MonsterGeneric>& monstersGenerics() const { return m_monstersGenerics; }

        //! Access the traps data base.
        inline const TrapsDB& trapsDB() const { return m_trapsDB; }

        //! Access the traps generics.
        inline const std::unordered_map<std::wstring, TrapGeneric>& trapsGenerics() const { return m_trapsGenerics; }

        //! Access the heroes data base.
        inline const HeroesDB& heroesDB() const { return m_heroesDB; }

        //! Access the associated villain.
        inline context::Villain& villain() { return *m_villain; }

        //! Access the monsters manager.
        inline MonstersManager& monstersManager() { return m_monstersManager; }

        //! Access the heroes manager.
        inline HeroesManager& heroesManager() { return m_heroesManager; }

        //! Access the dynamics manager.
        inline DynamicsManager& dynamicsManager() { return m_dynamicsManager; }

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
        //! @access floorRoomsCount setFloorRoomsCount changedFloorRoomsCount
        PARAMGSU(uint, m_floorRoomsCount, floorRoomsCount, setFloorRoomsCount, changedFloorRoomsCount)

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

        //----------------------//
        //! @name Graph linking
        //! @{

        //! Set the graph to be linked with these data.
        void useGraph(Graph& graph);

        //! @}

        //--------------//
        //! @name Rooms
        //! @{

        //! Refresh the room hide flags from all elements inside and send an event if changed.
        void updateRoomHide(const RoomCoords& coords);

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Callback on floorCount property.
        void changedFloorsCount();

        //! Callback on floorRoomsCount property.
        void changedFloorRoomsCount();

        //! Correct the dungeon data.
        /*!
         *  Will resize the room/floor vectors to current values
         *  held in floorsCount and floorRoomsCount properties,
         *  and replace all RoomState::UNKNOWN to RoomState::EMPTY.
         */
        void correctFloorsRooms();

        //! @}

    private:

        Graph* m_graph = nullptr;               //!< The graph linked to these data.
        context::Villain* m_villain = nullptr;  //!< The villain reference.
        context::Wallet m_fameWallet;           //!< The resource fame value.
        context::Wallet m_soulWallet;           //!< The resource soul value.

        // Debt
        uint m_debtPerWeek = 0u;    //!< Amount of dosh disappearing each week.
        uint m_debtWeeksLeft = 0u;  //!< How long the debt is.

        // Managers
        HeroesManager m_heroesManager;      //!< Manage all heroes.
        MonstersManager m_monstersManager;  //!< Manage all monsters.
        DynamicsManager m_dynamicsManager;  //!< Manage all dynamic elements.

        // Dungeon structure
        std::vector<Floor> m_floors;    //!< A dungeon consists in a vector of floors.

        // Time
        uint m_time = 0u;           //!< How much time the dungeon has been constructed, in in-game hours.
        const float m_timeGameHour; //!< Constant time: how many real seconds equals an in-game hour.

        // Databases
        MonstersDB m_monstersDB;        //!< All monsters immuable data.
        TrapsDB m_trapsDB;              //!< All traps immuable data.
        FacilitiesDB m_facilitiesDB;    //!< All facilities immuable data.
        HeroesDB m_heroesDB;            //!< All heroes immuable data.

        // Generics
        std::unordered_map<std::wstring, TrapGeneric> m_trapsGenerics;          //!< More info about the trap's types.
        std::unordered_map<std::wstring, MonsterGeneric> m_monstersGenerics;    //!< More info about the monster's types.
    };
}

