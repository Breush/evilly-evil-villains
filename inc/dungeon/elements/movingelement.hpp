#pragma once

#include "dungeon/graph.hpp"
#include "dungeon/elements/element.hpp"

namespace dungeon
{
    //! A generic moving element interface.

    class MovingElement : public Element
    {
        using baseClass = Element;

        //! All the weights used by Lua algorithms.
        struct Weight
        {
            // The following are specific to the current AI.
            uint visited = 0u;      //!< How often the node has been visited.
            uint lastVisit = 0u;    //!< The tick of the last time the node has been visited.

            // Those are element-specific
            uint altitude = 0u;     //!< How high is the node.
            uint treasure = 0u;     //!< How many money there is stored in this node.
            bool exit = false;      //!< Whether the hero can exit the dungeon by this node or not.
        };

    public:

        //! Constructor.
        MovingElement(std::string folder, Inter& inter, Graph& graph);

        //! Default destructor.
        virtual ~MovingElement() = default;

        //---------------------//
        //! @name Element data
        //! @{

        //! Rebinds the element data.
        void bindElementData(ElementData& edata);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void updateAI(const sf::Time& dt) final;
        void updateRoutine(const sf::Time& dt) final;

        //! @}

        //----------------//
        //! @name Lua API
        //! @{

        //! Will stop the entity to move and won't call for nextNode anymore.
        void lua_stopMoving();

        //! Returns true if the current direction is matching.
        bool lua_isLookingDirection(const std::string& direction) const;

        //! Get the current room x coordinate.
        uint lua_getCurrentRoomX() const;

        //! Get the current room y coordinate.
        uint lua_getCurrentRoomY() const;

        //! @}

        //--------------------------------//
        //! @name Artificial intelligence
        //! @{

        //! Refresh the position from the graph.
        void updateFromGraph();

        //! Convert a node to a node data.
        const Graph::NodeData* toNodeData(const ai::Node* node);

        //! Reinits the state to start a fresh new run.
        void reinit();

        //! Updates the AI.
        const Graph::NodeData* findNextNode(const Graph::NodeData* currentNode);

        //! Select the node to move the hero to.
        void setCurrentNode(const ai::Node* node);

        //! Get all weight information from a node.
        Weight getWeight(const Graph::NodeData* node);

        //! Returns the evaluation of a Lua function given a node.
        uint call(const char* function, const Graph::NodeData* node);

        //! @}

        //--------------------------------//
        //! @name Internal changes update
        //! @{

        //! Set the target position and update the eData accordingly.
        void setNewTargetPosition(const sf::Vector2f& targetPosition);

        //! Recompute the target position.
        void refreshPositionFromNode();

        //! Refresh the state of the walk animation.
        void refreshAnimation();

        //! @}

        //! A node extra information.
        struct NodeInfo
        {
            uint visits = 0u;           //!< How many times the node has been visited.
            uint16 lastVisit = 0x7FFF;  //!< The tick of the last time the node has been visited.
        };

    private:

        Graph& m_graph;                 //!< Abstract dungeon graph.
        std::string m_folder;           //!< The folder name where to find.
        std::wstring m_monsterID;       //!< Current monster ID.

        // Graph evaluation for AI
        uint m_tick = 0u;                                       //!< The current tick (how many nodes has been visited so far).
        std::unordered_map<sf::Vector2u, NodeInfo> m_nodeInfos; //!< Remembers the visits of a certain node.
        std::vector<int> m_evaluations;                         //!< Stores the evaluations of the rooms, mainly used for debug.

        // Artificial intelligence
        const ai::Node* m_currentNode = nullptr;    //!< The current room where is this monster.
        bool m_left = false;                        //!< Is the creepim looking left?
        bool m_moving = true;                       //!< Is the monster still tries to evaluate next room?
        float m_pauseTime = -1.f;                   //!< Current time waiting for delay.
        float m_pauseDelay = 0.f;                   //!< How many seconds to stay still if not going to next node.
    };
}
