#pragma once

#include "scene/entity.hpp"
#include "dungeon/graph.hpp"
#include "dungeon/elements/hero.hpp"
#include "context/event.hpp"
#include "scene/wrappers/label.hpp"
#include "scene/wrappers/animatedsprite.hpp"
#include "scene/wrappers/rectangleshape.hpp"
#include "ai/debug.hpp"

namespace dungeon
{
    // Forward declarations

    class Data;
    class Inter;

    //! A hero invading the dungeon.

    class HeroesManager final : public context::EventReceiver
    {
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

        //------------------------//
        //! @name Heroes feedback
        //! @{

        //! Feedback from Hero when he gets out of the dungeon.
        void heroGetsOut(Hero* hero);

        //! Feedback from Hero when he tries to steal a treasure.
        uint heroStealsTreasure(Hero* hero, const sf::Vector2u& coords, const uint stolenDosh);

        //! @}

    protected:

        //---------------//
        //! @name Events
        //! @{

        void receive(const context::Event& event) final;

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

        //! @}

    private:

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
            HeroStatus status = HeroStatus::TO_SPAWN;   //!< Hero status.
            float spawnDelay = 0.f;                     //!< Seconds to wait before effective spawning.
            ElementData data;                           //!< All its data.
            bool reward = false;                        //!< Do the player get a reward when this hero is removed?
        };

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
