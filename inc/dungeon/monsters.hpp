#pragma once

#include "scene/entity.hpp"
#include "scene/grabbable.hpp"
#include "scene/wrappers/richlabel.hpp"
#include "dungeon/elementdata.hpp"
#include "context/event.hpp"
#include "ai/dumbpuppet.hpp"
#include "tools/param.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace dungeon
{
    // Forward declarations

    class Data;
    class Inter;
    class Graph;

    //! A generic monster interface.
    class Monster : public scene::Entity, public context::EventReceiver
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        //! Set the reference to the room in data.
        Monster(sf::Vector2u& coords, ElementData& elementdata, dungeon::Inter& inter)
            : baseClass(true)
            , m_active(false)
            , m_coords(coords)
            , m_inter(inter)
            , m_elementdata(elementdata)
        {
            setDetectable(false);
        }

        //! Default destructor.
        virtual ~Monster() = default;

        virtual std::string _name() const override = 0;

        //--------------------------//
        //! @name Public properties
        //! @{

        //! To be set to false when monster should stop.
        PARAMGSU(bool, m_active, active, setActive, refreshFromActivity)

        //! @}

        //--------------//
        //! @name Graph
        //! @{

        //! The graph of the dungeon to be read from.
        virtual void useGraph(Graph& graph) = 0;

        //! @}

    protected:

        //---------------//
        //! @name Events
        //! @{

        virtual void receive(const context::Event& event) override = 0;

        //! @}

        //----------------//
        //! @name Events
        //! @{

        //! Detect if any hero is nearby this entity.
        //! @param relRange is expressed relatively to room size.
        bool isHeroNearby(float relRange) const;

        //! @}

        //--------------------------------//
        //! @name Internal changes update
        //! @{

        //! Activity status changed.
        virtual void refreshFromActivity() = 0;

        //! @}

    protected:

        sf::Vector2u& m_coords;         //!< The room in which the monster is set.
        dungeon::Inter& m_inter;        //!< To be able to interact with nearby elements.
        ElementData& m_elementdata;     //!< The data corresponding to the monster.
    };

    //! A MonsterGrabbable spawner.
    /*!
     *  Implements the scene::GrabbableSpawner for a monster reserve.
     *  The player gabbs and moves the monster into the dungeon.
     */

    class MonsterCage final : public scene::Entity, public scene::GrabbableSpawner, public context::EventReceiver
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor, affecting texture and look.
        MonsterCage(std::wstring monsterID, Data& data);

        //! Default destructor.
        ~MonsterCage() = default;

        std::string _name() const final { return "dungeon::MonsterCage"; }

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void refreshNUI(const config::NUIGuides& cNUI) override;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        void receive(const context::Event& event) final;
        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;

        //! @}

        //------------------------//
        //! @name Spawn and react
        //! @{

        void grabbableButtonReleased(Entity* entity, const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos) final;
        std::unique_ptr<scene::Grabbable> spawnGrabbable() final;

        //! @}

        //------------------------//
        //! @name Spawn and react
        //! @{

        //! Refresh the cost labels colors given the current dosh held by the player.
        void refreshCostLabelsColor();

        //! @}

    private:

        Data& m_data;               //!< Reference to the whole data.
        std::wstring m_monsterID;   //!< The monster to be moved into the dungeon inter.

        // Decorum
        sf::RectangleShape m_background;    //!< Background.
        ai::DumbPuppet m_monsterPuppet;     //!< Puppet that moves from left to right.
        scene::RichLabel m_baseCostLabel;   //!< Text for base cost.
    };

    //! A monster temporary object.
    /*!
     *  Implements the scene::Grabbable for the a dungeon monster.
     */

    class MonsterGrabbable final : public scene::Grabbable
    {
        using baseClass = scene::Grabbable;

    public:

        //! Constructor, affecting texture and look.
        MonsterGrabbable(scene::GrabbableSpawner& spawner, const std::wstring& monsterID);

        //! Default destructor.
        ~MonsterGrabbable() = default;

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

        //! @}

    private:

        //! Holding the image of the grabbable.
        scene::AnimatedSprite m_sprite;
    };
}
