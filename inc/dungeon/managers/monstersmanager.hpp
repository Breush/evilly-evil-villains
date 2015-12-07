#pragma once

#include "scene/entity.hpp"
#include "dungeon/graph.hpp"
#include "dungeon/elements/monster.hpp"
#include "context/event.hpp"

namespace dungeon
{
    // Forward declarations

    class Data;
    class Inter;

    //! Monsters managed in the dungeon.

    class MonstersManager final : public context::EventReceiver
    {
    public:

        //! Constructor.
        MonstersManager();

        //! Default destructor.
        ~MonstersManager() = default;

        //----------------//
        //! @name Routine
        //! @{

        //! Update routine.
        void update(const sf::Time& dt);

        //! @}

        //-------------------------//
        //! @name File interaction
        //! @{

        //! Load from an XML file.
        void load(const pugi::xml_node& node);

        //! Save to an XML file.
        void save(pugi::xml_node node);

        //! @}

        //----------------------------//
        //! @name Dungeon interaction
        //! @{

        //! The graph of the dungeon to be read from.
        void useGraph(Graph& graph);

        //! Set the dungeon inter source.
        void useInter(Inter& inter);

        //! @}

        //----------------------------//
        //! @name Dungeon interaction
        //! @{

        //! Remove the monsters in the room.
        void removeRoomMonsters(const sf::Vector2u& coords);

        //! Add the monster in the room.
        void addRoomMonster(const sf::Vector2u& coords, const std::wstring& monsterID);

        //! @}

    protected:

        //---------------//
        //! @name Events
        //! @{

        void receive(const context::Event& event) final;

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Refresh all element data reference for heroes.
        void refreshMonstersData();

        //! @}

    private:

        //! Basic monster status, modifications occurs during update.
        enum class MonsterStatus
        {
            RUNNING,        //!< Monster is running inside the dungeon, standard status.
            TO_BE_REMOVED,  //!< Monster will be removed during next update.
            TO_SPAWN,       //!< Monster will spawn.
        };

        //! Contains a Monster plus its status.
        struct MonsterInfo
        {
            std::unique_ptr<Monster> monster = nullptr;     //!< Hero pointer.
            MonsterStatus status = MonsterStatus::TO_SPAWN; //!< Hero status.
            ElementData data;                               //!< All its data.
        };

    private:

        // Shared data
        Data* m_data = nullptr;     //!< Dungeon data.
        Graph* m_graph = nullptr;   //!< The graph of the dungeon to be read from.
        Inter* m_inter = nullptr;   //!< The dungeon inter, to get cellsize and position.

        // Monsters
        std::vector<MonsterInfo> m_monstersInfo;    //!< All the heroes currently in the dungeon.
    };
}
