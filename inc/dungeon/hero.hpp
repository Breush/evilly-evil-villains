#pragma once

#include "scene/entity.hpp"
#include "dungeon/graph.hpp"
#include "dungeon/event.hpp"
#include "sfe/label.hpp"
#include "sfe/animatedsprite.hpp"

#include <selene/selene.hpp>

namespace dungeon
{
    // Forward declarations

    class Data;
    class Inter;

    //! A hero invading the dungeon.

    class Hero final : public scene::Entity, public EventReceiver
    {
        using baseClass = scene::Entity;

    public:

        //! All the weights used by Lua algorithms.
        struct Weight
        {
            // The following are specific to the current IA.
            uint visited = 0u;      //!< How often the node has been visited.
            uint lastVisit = 0u;    //!< The tick of the last time the node has been visited.

            // The following are coming from the graph information.
            uint altitude = 0u;     //!< How high is the node.
            uint treasure = 0u;     //!< How many money there is stored in this node.
            bool exit = false;      //!< Whether the hero can exit the dungeon by this node or not.
        };

    public:

        //! Constructor.
        Hero(const Inter* inter);

        //! Default destructor.
        ~Hero() = default;

        //----------------------//
        //! @name Dungeon graph
        //! @{

        //! The graph of the dungeon to be read from.
        void useGraph(Graph& data);

        //! @}

        //--------------------//
        //! @name Data events
        //! @{

        //! Set the dungeon data source.
        void useData(Data& data);

        //! @}

        //------------------//
        //! @name Resources
        //! @{

        //! Add a certain amount of dosh to current resources.
        inline void addDosh(uint amount) { setDosh(m_dosh + amount); }

        //! Substract a certain amount of dosh to current resources.
        inline void subDosh(uint amount) { setDosh(m_dosh - amount); }

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! Whether the hero is visible and active.
        PARAMGSU(bool, m_running, running, setRunning, changedRunning);

        //!< How much dosh the hero holds currently.
        PARAMGSU(uint, m_dosh, dosh, setDosh, changedDosh);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void updateAI(const sf::Time& dt) final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        void receive(const Event& event) final;

        //! @}

        //--------------------------------//
        //! @name Artificial intelligence
        //! @{

        //! Returns the evaluation of a Lua function given a node.
        uint call(const char* function, const Graph::Node* node);

        //! Called by the AI when it wants to get out.
        //! Will be accepted only if there is a door.
        void AIGetOut();

        //! Called by the AI when it wants to steal money from treasure.
        void AIStealTreasure();

        //! @}

        //------------------------//
        //! @name Node management
        //! @{

        //! Select the node to move the hero to.
        //! Set firstNode to true if the hero enters the dungeon via this node.
        void setCurrentNode(const Graph::Node* node, bool firstNode = false);

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Recompute the local position of the hero.
        //! Set firstNode to true if the hero enters the dungeon via this node.
        void refreshPositionFromNode(bool firstNode = false);

        //! Whenever the running mode changed.
        void changedRunning();

        //! Whenever the dosh changed.
        void changedDosh();

        //! @}

        //! A node extra information.
        struct NodeInfo
        {
            uint visits = 0u;           //!< How many times the node has been visited.
            uint16 lastVisit = 0x7FFF;  //!< The tick of the last time the node has been visited.
        };

    private:

        Data* m_data = nullptr; //!< Dungeon data.

        // The graph
        Graph* m_graph = nullptr;                   //!< The graph of the dungeon to be read from.
        const Graph::Node* m_currentNode = nullptr; //!< The current room where is our hero.
        const Inter* m_inter = nullptr;             //!< The dungeon inter, to get cellsize and position.

        // Artificial intelligence
        sel::State m_lua;               //!< The lua state.
        uint m_tick = 0u;               //!< The current tick (how many nodes has been visited so far).
        std::unordered_map<sf::Vector2u, NodeInfo> m_nodeInfos; //!< Remembers the visits of a certain node.

        // Decorum
        sfe::Label m_doshLabel;         //!< How much dosh held by the hero.
        sfe::AnimatedSprite m_sprite;   //!< The sprite of the hero.
    };
}
