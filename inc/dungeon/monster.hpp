#pragma once

#include "dungeon/graph.hpp"
#include "dungeon/elementdata.hpp"
#include "dungeon/detectentity.hpp"
#include "scene/wrappers/animatedsprite.hpp"
#include "scene/wrappers/rectangleshape.hpp"

#include <selene/selene.hpp>

namespace dungeon
{
    // Forward declarations

    class Inter;

    //! A generic monster interface.

    class Monster final : public DetectEntity
    {
        using baseClass = DetectEntity;

        //! All the weights used by Lua algorithms.
        struct Weight
        {
            // The following are specific to the current AI.
            uint visited = 0u;      //!< How often the node has been visited.
            uint lastVisit = 0u;    //!< The tick of the last time the node has been visited.

            // Those are monster-specific
            uint altitude = 0u;     //!< How high is the node.
            uint treasure = 0u;     //!< How many money there is stored in this node.
            bool exit = false;      //!< Whether the hero can exit the dungeon by this node or not.
        };

    public:

        //! Constructor.
        Monster(Inter& inter, Graph& graph);

        //! Default destructor.
        ~Monster() = default;

        std::string detectKey() const final { return "monster"; }
        std::string _name() const final { return "dungeon::Monster"; }

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

        //! Calling detector.
        void lua_addCallback(const std::string& luaKey, const std::string& entityType, const std::string& condition);

        //! Init the eData with the value if empty.
        void lua_initEmptyDataFloat(const std::string& s, const lua_Number value);

        //! Set the eData with the value specified.
        void lua_setDataFloat(const std::string& s, const lua_Number value);

        //! Get the correponding eData.
        lua_Number lua_getDataFloat(const std::string& s) const;

        //! Select an animation to play.
        void lua_selectAnimation(const std::string& animationKey);

        //! Set the looping paramter of the current animation.
        void lua_setAnimationLooping(const bool looping);

        //! Will stop the entity to move and won't call for nextNode anymore.
        void lua_stopMoving();

        //! Returns true if the current direction is matching.
        bool lua_isLookingDirection(const std::string& direction) const;

        //! Returns true if the current animation has stopped (looping has to be false).
        bool lua_isAnimationStopped() const;

        //! Advance the current animation from a certain offset.
        void lua_forwardAnimation(const lua_Number offset);

        //! Restart the current animation from the beginning.
        void lua_rewindAnimation();

        //! Get the current room x coordinate.
        uint lua_getCurrentRoomX() const;

        //! Get the current room y coordinate.
        uint lua_getCurrentRoomY() const;

        //! Debug log function from lua.
        void lua_log(const std::string& str) const;

        //! Explode the corresponding room.
        void lua_dungeonExplodeRoom(const uint x, const uint y);

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

    protected:

        Inter& m_inter;                 //!< To be able to interact with nearby elements.
        Graph& m_graph;                 //!< Abstract dungeon graph.
        ElementData* m_edata = nullptr; //!< The data corresponding to the monster.

        std::wstring m_monsterID;       //!< Current monster ID.

        // Graph evaluation for AI
        sel::State m_lua;                                       //!< The lua state.
        uint m_tick = 0u;                                       //!< The current tick (how many nodes has been visited so far).
        std::unordered_map<sf::Vector2u, NodeInfo> m_nodeInfos; //!< Remembers the visits of a certain node.
        std::vector<int> m_evaluations;                         //!< Stores the evaluations of the rooms, mainly used for debug.

        // Decorum
        scene::AnimatedSprite m_sprite;   //!< The sprite.

        // Artificial intelligence
        const ai::Node* m_currentNode = nullptr;    //!< The current room where is this monster.
        bool m_left = false;                        //!< Is the creepim looking left?
        bool m_moving = true;                       //!< Is the monster still tries to evaluate next room?
        float m_pauseTime = -1.f;                   //!< Current time waiting for delay.
        float m_pauseDelay = 0.f;                   //!< How many seconds to stay still if not going to next node.
    };
}
