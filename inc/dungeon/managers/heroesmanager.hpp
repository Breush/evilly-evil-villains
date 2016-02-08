#pragma once

#include "scene/entity.hpp"
#include "dungeon/graph.hpp"
#include "dungeon/elements/hero.hpp"
#include "context/event.hpp"

namespace dungeon
{
    // Forward declarations

    class Data;
    class Inter;

    //! A hero invading the dungeon.

    class HeroesManager final : public context::EventReceiver
    {
        //! Basic hero status, modifications occurs during update.
        enum class HeroStatus
        {
            RUNNING,        //!< Hero is running inside the dungeon, standard status.
            TO_BE_REMOVED,  //!< Hero will be removed during next update.
            TO_SPAWN,       //!< Hero will spawn, delay indicated into data.
        };

        //! Contains an Hero plus its status.
        struct HeroInfo
        {
            std::unique_ptr<Hero> hero = nullptr;       //!< Hero pointer.

            // Saved states
            ElementData data;                           //!< All its data.
            HeroStatus status = HeroStatus::TO_SPAWN;   //!< Hero status.
            float spawnDelay = 0.f;                     //!< Seconds to wait before effective spawning.
            float hp = 0.f;                             //!< How many HP the hero has left.

            // Not saved states
            bool spawnHard = false;                     //!< Is the spawn point already defined?
            bool locked = false;                        //!< Can the hero be removed?
            bool damageFeedback = false;                //!< Are we showing a damage animation?
            float damageFeedbackTime = 0.f;             //!< How long to wait for the damage state.
        };

    public:

        //! Constructor.
        HeroesManager();

        //! Default destructor.
        ~HeroesManager() = default;

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

        //-----------------------//
        //! @name Heroes control
        //! @{

        //! Damage the hero specified.
        void damage(const Hero* hero, float amount);

        //! Whether to lock a hero from other changes (won't be removed).
        void setLocked(const Hero* hero, bool locked);

        //! Set the list to the room heroes.
        void listRoomHeroes(const RoomCoords& coords, std::vector<Hero*>& heroesList) const;

        //! Mark all heroes in that room to be removed.
        void removeRoomHeroes(const RoomCoords& coords);

        //! @}

        //------------------------//
        //! @name Heroes feedback
        //! @{

        //! Feedback from Hero when he gets out of the dungeon.
        void heroGetsOut(Hero* hero);

        //! Feedback from Hero when he tries to steal a treasure.
        uint heroStealsTreasure(Hero* hero, const RoomCoords& coords, const uint stolenDosh);

        //! @}

    protected:

        //---------------//
        //! @name Events
        //! @{

        void receive(const context::Event& event) final;

        //! @}

        //--------------------------//
        //! @name Heroes management
        //! @{

        //! Create the hero pointer and bind it to its data.
        void createHeroObject(HeroInfo& heroInfo);

        //! @}

        //--------------------------------//
        //! @name Artificial intelligence
        //! @{

        //! Convert a node to a node data.
        const Graph::NodeData* toNodeData(const ai::Node* node);

        //! Spawns a new random group of heroes.
        void spawnHeroesGroup();

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Refresh all element data reference for heroes.
        void refreshHeroesData();

        //! Refresh all heroes information about the current graph.
        void refreshHeroesFromGraph();

        //! @}

    private:

        // Shared data
        Data* m_data = nullptr;     //!< Dungeon data.
        Graph* m_graph = nullptr;   //!< The graph of the dungeon to be read from.
        Inter* m_inter = nullptr;   //!< The dungeon inter, to get cellsize and position.

        // Heroes
        std::vector<HeroInfo> m_heroesInfo; //!< All the heroes currently in the dungeon.
        float m_nextGroupDelay = -1.f;
    };
}
