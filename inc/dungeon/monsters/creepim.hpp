#pragma once

#include "dungeon/monsters.hpp"
#include "sfe/animatedsprite.hpp"
#include "sfe/rectangleshape.hpp"
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
        Creepim(const sf::Vector2u& coords, ElementData& elementdata, dungeon::Inter& inter);

        //! Default destructor.
        ~Creepim() = default;

        //--------------//
        //! @name Graph
        //! @{

        void useGraph(Graph& graph) final;

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void updateAI(const sf::Time& dt) final;
        void onTransformChanges() final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        void receive(const Event& event) final;

        //! @}

        //------------------------//
        //! @name Node management
        //! @{

        //! Select the node to move the hero to.
        void setCurrentNode(const Graph::Node* node);

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
        sfe::AnimatedSprite m_sprite;   //!< The sprite.

        // Artificial intelligence
        ai::LuaActor m_luaActor;                    //!< Loads lua file and move into the graph.
        const Graph::Node* m_currentNode = nullptr; //!< The current room where is this monster.
    };
}
}
