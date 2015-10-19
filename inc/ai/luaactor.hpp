#pragma once

#include "dungeon/graph.hpp"

#include <selene/selene.hpp>

// TODO This LUA Actor can not exist... or should be in dungeon.

namespace ai
{
    //! Actor that interfaces with Lua.
    /*!
     *  Lua file will have to feature init(), evaluate_reference()
     *  and evaluate() procedures.
     */

    class LuaActor
    {
        using Graph = dungeon::Graph;
        using Node = dungeon::Graph::NodeData;

    public:

        //! All the weights used by Lua algorithms.
        struct Weight
        {
            // The following are specific to the current AI.
            uint visited = 0u;      //!< How often the node has been visited.
            uint lastVisit = 0u;    //!< The tick of the last time the node has been visited.

            // TODO How can this be defined inside AI? Should be dynamically set somehow.
            // The following are coming from the graph information.
            uint altitude = 0u;     //!< How high is the node.
            uint treasure = 0u;     //!< How many money there is stored in this node.
            bool exit = false;      //!< Whether the hero can exit the dungeon by this node or not.
        };

    public:

        //! Default constructor.
        LuaActor() = default;

        //! Default destructor.
        virtual ~LuaActor() = default;

        //----------------//
        //! @name Control
        //! @{

        //! Reinits the state to start a fresh new run.
        void reinit();

        //! Updates the AI.
        const Node* findNextNode(const Node* currentNode);

        //! @}

        //----------------------//
        //! @name Configuration
        //! @{

        //! Sets the lua file.
        void loadFile(const std::string& filename);

        //! The graph of the dungeon to be read from.
        void useGraph(Graph& graph);

        //! Access the lua state.
        inline sel::State& state() { return m_lua; }

        //! @}

        //--------------------------------//
        //! @name Artificial intelligence
        //! @{

        //! Get all weight information from a node.
        Weight getWeight(const Node* node);

        //! Get the evaluation details of a last update.
        /*!
         *  Index 0u is currentNode evaluation.
         *  Other indexes are neighbours.
         */
        inline int evaluation(uint index) const { return m_evaluations[index]; }

        //! @}

    protected:

        //--------------------------------//
        //! @name Artificial intelligence
        //! @{

        //! Returns the evaluation of a Lua function given a node.
        uint call(const char* function, const Node* node);

        //! @}

        //! A node extra information.
        struct NodeInfo
        {
            uint visits = 0u;           //!< How many times the node has been visited.
            uint16 lastVisit = 0x7FFF;  //!< The tick of the last time the node has been visited.
        };

    private:

        Graph* m_graph = nullptr;       //!< The graph to be read from.

        sel::State m_lua;               //!< The lua state.
        uint m_tick = 0u;               //!< The current tick (how many nodes has been visited so far).
        std::unordered_map<sf::Vector2u, NodeInfo> m_nodeInfos; //!< Remembers the visits of a certain node.
        std::vector<int> m_evaluations; //!< Stores the evaluations of the rooms, mainnly used for debug.
    };
}
