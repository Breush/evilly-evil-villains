#pragma once

#include "dungeon/graph.hpp"
#include "dungeon/monsters.hpp"
#include "scene/wrappers/animatedsprite.hpp"
#include "scene/wrappers/rectangleshape.hpp"

#include <selene/selene.hpp>

namespace dungeon
{
namespace monsters
{
    // TODO Transform it into a generic monster class
    //!< A creepim explodes when there are nearby heroes.

    class Creepim final : public Monster
    {
        using baseClass = Monster;

        //! All the weights used by Lua algorithms.
        struct Weight
        {
            // The following are specific to the current AI.
            uint visited = 0u;      //!< How often the node has been visited.
            uint lastVisit = 0u;    //!< The tick of the last time the node has been visited.

            // TODO See LuaActor problem
            uint altitude = 0u;     //!< How high is the node.
            uint treasure = 0u;     //!< How many money there is stored in this node.
            bool exit = false;      //!< Whether the hero can exit the dungeon by this node or not.
        };

    public:

        //! Constructor.
        Creepim(ElementData& elementdata, dungeon::Inter& inter);

        //! Default destructor.
        ~Creepim() = default;

        std::string _name() const final { return "dungeon::monsters::Creepim"; }

        //--------------//
        //! @name Graph
        //! @{

        void useGraph(Graph& graph) final;

        //! Convert a node to a node data.
        const Graph::NodeData* toNodeData(const ai::Node* node);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void updateAI(const sf::Time& dt) final;
        void updateRoutine(const sf::Time& dt) final;
        void onTransformChanges() final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        inline void receive(const context::Event& event) final {}

        //! @}

        //------------------------//
        //! @name LUA interaction
        //! @{

        // TODO Move to a generic Detecter from some GameEntity
        void lua_addCallback(const std::string& luaKey, const std::string& entityType, const std::string& condition);

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

        // TODO Find a better way to get those?
        // Note: sel::Tuple<uint, uint> fail, struct personnel fail, (uint&, uint&) fail
        //! Get the current room coordinates.
        uint lua_getCurrentRoomX() const;
        uint lua_getCurrentRoomY() const;

        //! Debug log function from lua.
        void lua_log(const std::string& str) const;

        //! Explode the corresponding room.
        void lua_dungeonExplodeRoom(const uint x, const uint y);

        //! @}

        //------------------------//
        //! @name Node management
        //! @{

        //! Updates the AI.
        const dungeon::Graph::NodeData* findNextNode(const dungeon::Graph::NodeData* currentNode);

        //! Select the node to move the hero to.
        void setCurrentNode(const ai::Node* node);

        //! @}

        //--------------------------------//
        //! @name Artificial intelligence
        //! @{

        //! Reinits the state to start a fresh new run.
        void reinit();

        //! Get all weight information from a node.
        Weight getWeight(const dungeon::Graph::NodeData* node);

        //! Returns the evaluation of a Lua function given a node.
        uint call(const char* function, const dungeon::Graph::NodeData* node);

        //! @}

        //--------------------------------//
        //! @name Internal changes update
        //! @{

        //! Recompute the local position.
        //! Setting teleport to true will reset the lerpable.
        void refreshPositionFromNode();

        //! Refresh the monster status whenever activity changes.
        void refreshFromActivity() final;

        //! @}

        //! A node extra information.
        struct NodeInfo
        {
            uint visits = 0u;           //!< How many times the node has been visited.
            uint16 lastVisit = 0x7FFF;  //!< The tick of the last time the node has been visited.
        };

    private:

        dungeon::Graph* m_graph = nullptr;  //!< Abstract dungeon graph.

        // Graph evaluation for AI
        sel::State m_lua;               //!< The lua state.
        uint m_tick = 0u;               //!< The current tick (how many nodes has been visited so far).
        std::unordered_map<sf::Vector2u, NodeInfo> m_nodeInfos; //!< Remembers the visits of a certain node.
        std::vector<int> m_evaluations; //!< Stores the evaluations of the rooms, mainnly used for debug.

        // Decorum
        scene::AnimatedSprite m_sprite;   //!< The sprite.

        // Artificial intelligence
        const ai::Node* m_currentNode = nullptr;    //!< The current room where is this monster.
        bool m_left = false;                        //!< Is the creepim looking left?
        bool m_moving = true;                       //!< Is the monster still tries to evaluate next room?
    };
}
}
