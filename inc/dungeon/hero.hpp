#pragma once

#include "dungeon/detectentity.hpp"
#include "scene/wrappers/animatedsprite.hpp"
#include "scene/wrappers/rectangleshape.hpp"
#include "scene/wrappers/label.hpp"
#include "ai/luaactor.hpp"
#include "ai/debug.hpp"

namespace dungeon
{
    // Forward declarations

    class HeroesManager;
    class Inter;

    //! A hero invading the dungeon.

    class Hero final : public DetectEntity
    {
        using baseClass = DetectEntity;

    public:

        //! Constructor.
        Hero(HeroesManager& manager, Inter& inter);

        //! Default destructor.
        ~Hero() = default;

        std::string detectKey() const final { return "hero"; }
        std::string _name() const final { return "dungeon::Hero"; }

        //------------------//
        //! @name Resources
        //! @{

        //! Add a certain amount of dosh to current resources.
        inline void addDosh(uint amount) { setDosh(m_dosh + amount); }

        //! Substract a certain amount of dosh to current resources.
        inline void subDosh(uint amount) { setDosh(m_dosh - amount); }

        //! @}

        //--------------//
        //! @name Graph
        //! @{

        //! The graph of the dungeon to be read from.
        void useGraph(Graph& graph);

        //! Convert a node to a node data.
        const Graph::NodeData* toNodeData(const ai::Node* node);

        //! @}

        //----------------//
        //! @name Getters
        //! @{

        //! The graph of the dungeon to be read from.
        inline const ai::Node* currentNode() const { return m_currentNode; }

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //!< How much dosh the hero holds currently.
        PARAMGSU(uint, m_dosh, dosh, setDosh, changedDosh);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onTransformChanges() final;
        void updateAI(const sf::Time& dt) final;

        //! @}

        //--------------------------------//
        //! @name Artificial intelligence
        //! @{

        //! Called by the AI when it wants to get out.
        //! Will be accepted only if there is an entrance.
        void AIGetOut();

        //! Called by the AI when it wants to steal money from treasure.
        void AIStealTreasure();

        //! @}

        //------------------------//
        //! @name Node management
        //! @{

        //! Select the node to move the hero to.
        void setCurrentNode(const ai::Node* node);

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Recompute the local position of the hero.
        //! Set firstNode to true if the hero enters the dungeon via this node.
        void refreshPositionFromNode(bool firstNode = false);

        //! Whenever the dosh changed.
        void changedDosh();

        #if DEBUG_AI > 0
        //! Refresh an overlay position and content.
        void refreshDebugOverlay(uint index, const ai::Node* node);

        //! Refresh the overlay to current node and nearby ones.
        void refreshDebugOverlays();
        #endif

        //! @}

    private:

        // The graph
        HeroesManager& m_manager;                   //!< The heroes manager for feedback.
        const ai::Node* m_currentNode = nullptr;    //!< The current room where is our hero.
        Inter& m_inter;                             //!< The dungeon inter, to get cellsize and position.

        // Artificial intelligence
        ai::LuaActor m_luaActor;        //!< Loads lua file and move into the graph.

        // Decorum
        scene::Label m_doshLabel;         //!< How much dosh held by the hero.
        scene::AnimatedSprite m_sprite;   //!< The sprite of the hero.

        // Debug overlay
        #if DEBUG_AI > 0
        std::array<scene::RectangleShape, 5u> m_overlays; //!< Backgrounds, one for each direction plus one central.
        std::array<scene::Label, 5u> m_overlayLabels;     //!< Text over backgrounds.
        #endif
    };
}
