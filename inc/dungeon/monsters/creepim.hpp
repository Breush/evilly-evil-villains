#pragma once

#include "dungeon/monsters.hpp"
#include "scene/wrappers/animatedsprite.hpp"
#include "scene/wrappers/rectangleshape.hpp"
#include "ai/luaactor.hpp"

namespace dungeon
{
namespace monsters
{
    //!< A creepim explodes when there are nearby heroes.

    class Creepim final : public Monster
    {
        using baseClass = Monster;

    public:

        //! Constructor.
        Creepim(sf::Vector2u& coords, ElementData& elementdata, dungeon::Inter& inter);

        //! Default destructor.
        ~Creepim() = default;

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

        //------------------//
        //! @name Detecter
        //! @{

        /* void onDetecterCallback(const std::string& key) final; */

        //! @}

        //------------------------//
        //! @name Node management
        //! @{

        //! Select the node to move the hero to.
        void setCurrentNode(const ai::Node* node);

        //! @}

        //--------------------------------//
        //! @name Internal changes update
        //! @{

        //! Recompute the local position.
        //! Setting teleport to true will reset the lerpable.
        void refreshPositionFromNode(bool teleport = false);

        //! Refresh the monster status whenever activity changes.
        void refreshFromActivity() final;

        //! @}

    private:

        dungeon::Graph* m_graph = nullptr;  //!< Abstract dungeon graph.

        // Decorum
        scene::AnimatedSprite m_sprite;   //!< The sprite.

        // Artificial intelligence
        ai::LuaActor m_luaActor;                    //!< Loads lua file and move into the graph.
        const ai::Node* m_currentNode = nullptr;    //!< The current room where is this monster.
        bool m_left = false;                        //!< Is the creepim looking left?

        // Exploding
        bool m_fusing = false;          //!< Is the creepim going to explode?
    };
}
}
