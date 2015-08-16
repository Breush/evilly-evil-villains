#pragma once

#include "scene/entity.hpp"
#include "dungeon/graph.hpp"
#include "dungeon/event.hpp"
#include "dungeon/hero.hpp"
#include "sfe/label.hpp"
#include "sfe/animatedsprite.hpp"
#include "sfe/rectangleshape.hpp"
#include "ai/debug.hpp"

#include <selene/selene.hpp>

namespace dungeon
{
    // Forward declarations

    class Data;
    class Inter;

    //! A hero invading the dungeon.

    class HeroesManager final : public EventReceiver
    {
    public:

        //! Constructor.
        HeroesManager(Inter& inter);

        //! Default destructor.
        ~HeroesManager() = default;

        //----------------//
        //! @name Routine
        //! @{

        //! Update routine.
        void update(const sf::Time& dt);

        //! @}

        //----------------------------//
        //! @name Dungeon interaction
        //! @{

        //! The graph of the dungeon to be read from.
        void useGraph(Graph& graph);

        //! Set the dungeon data source.
        void useData(Data& data);

        //! @}

        //------------------------//
        //! @name Heroes feedback
        //! @{

        //! Feedback from Hero when he gets out of the dungeon.
        void heroGetsOut(Hero* hero);

        //! Feedback from Hero when he leaves a room.
        void heroLeftRoom(Hero* hero, const sf::Vector2u& coords);

        //! Feedback from Hero when he enters a room.
        void heroEnteredRoom(Hero* hero, const sf::Vector2u& coords);

        //! Feedback from Hero when he tries to steal a treasure.
        void heroStealsTreasure(Hero* hero, const sf::Vector2u& coords, const uint stolenDosh);

        //! @}

    protected:

        //---------------//
        //! @name Events
        //! @{

        void receive(const Event& event) final;

        //! @}

        //-----------------//
        //! @name Activity
        //! @{

        //! Spawns a new random group of heroes.
        void spawnHeroesGroup();

        //! Whether the new heroes are sent and dungeon active.
        void setActive(bool inActive);

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
            float data = 0.f;                           //!< Extra data information.
        };

    private:

        // Shared data
        Data* m_data = nullptr;     //!< Dungeon data.
        Graph* m_graph = nullptr;   //!< The graph of the dungeon to be read from.
        Inter& m_inter;             //!< The dungeon inter, to get cellsize and position.

        // Heroes
        std::vector<HeroInfo> m_heroesInfo; //!< All the heroes currently in the dungeon.
        float m_nextGroupDelay = -1.f;

        // Activity
        bool m_active = false;  //!< Whether the new heroes are sent and dungeon active.
    };
}