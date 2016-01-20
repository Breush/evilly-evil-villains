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

        //-------------------------//
        //! @name Monsters control
        //! @{

        //! Damage the monster specified.
        void damage(const Monster* monster, float amount);

        //! Remove the monsters in the room.
        void removeRoomMonsters(const sf::Vector2u& coords);

        //! Add the monster in the room.
        void addRoomMonster(const sf::Vector2u& coords, const std::wstring& monsterID);

        //! Whether to lock a monster from other changes (won't be removed).
        void setLocked(const Monster* monster, bool locked);

        //! Set the list to the room monsters.
        void listRoomMonsters(const sf::Vector2u& coords, std::vector<Monster*>& monstersList) const;

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

        //! Refresh all element data references.
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
            std::unique_ptr<Monster> monster = nullptr;     //!< Monster pointer.

            // Saved states
            ElementData data;                               //!< All its data.
            MonsterStatus status = MonsterStatus::TO_SPAWN; //!< Monster status.
            float hp = 0.f;                                 //!< How many HP the monster has left.

            // Not saved states
            bool locked = false;                            //!< Can the monster be removed?
            bool damageFeedback = false;                    //!< Are we showing a damage animation?
            float damageFeedbackTime = 0.f;                 //!< How long to wait for the damage state.
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
