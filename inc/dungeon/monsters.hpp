#pragma once

#include "scene/entity.hpp"
#include "scene/grabbable.hpp"
#include "dungeon/elementdata.hpp"
#include "dungeon/event.hpp"
#include "ai/dumbpuppet.hpp"
#include "tools/param.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace dungeon
{
    // Forward declarations

    class Inter;
    class Graph;

    //! A generic monster interface.
    class Monster : public scene::Entity, public EventReceiver
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        //! Set the reference to the room in data.
        Monster(const sf::Vector2u& coords, ElementData& elementdata, dungeon::Inter& inter)
            : baseClass(true)
            , m_active(false)
            , m_inter(inter)
            , m_coords(coords)
            , m_elementdata(elementdata)
        {
            setDetectable(false);
        }

        //! Default destructor.
        virtual ~Monster() = default;

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

        virtual void receive(const Event& event) override = 0;

        //! @}

        //--------------------------------//
        //! @name Internal changes update
        //! @{

        //! Activity status changed.
        virtual void refreshFromActivity() = 0;

        //! @}

    protected:

        dungeon::Inter& m_inter;        //!< To be able to interact with nearby elements.
        sf::Vector2u m_coords;          //!< The initial room in which the monster is set.
        ElementData& m_elementdata;     //!< The data corresponding to the monster.
    };

    //! A MonsterGrabbable spawner.
    /*!
     *  Implements the scene::GrabbableSpawner for a monster reserve.
     *  The player gabbs and moves the monster into the dungeon.
     */

    class MonsterCage final : public scene::Entity, public scene::GrabbableSpawner
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor, affecting texture and look.
        MonsterCage(std::wstring monsterID);

        //! Default destructor.
        ~MonsterCage() = default;

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        void handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;

        //! @}

        //------------------------//
        //! @name Spawn and react
        //! @{

        void grabbableButtonReleased(Entity* entity, const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos) final;
        std::unique_ptr<scene::Grabbable> spawnGrabbable() final;

        //! @}

    private:

        std::wstring m_monsterID;   //!< The monster to be moved into the dungeon inter.

        // Decorum
        sf::RectangleShape m_background;    //!< Background.
        ai::DumbPuppet m_monsterPuppet;     //!< Puppet that moves from left to right.
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
        MonsterGrabbable(scene::GrabbableSpawner& spawner);

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
        sf::RectangleShape m_sprite;
    };
}
