#pragma once

#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <memory>

namespace scene
{
    // Forward declarations

    class Graph;
    class Entity;
    class Grabbable;

    //! An abstract class to spawn a Grabbable.
    /*!
     *  Each Grabbable needs a spawner to call back when it will be
     *  destroyed. Derived classes specify which Grabbable to spawn (so that
     *  it can have any appearance you want) and decide what happens you
     *  Grabbable is released.
     */

    class GrabbableSpawner
    {
        friend class Graph;

    public:

        //! Default constructor.
        GrabbableSpawner() = default;

        //! Default destructor.
        virtual ~GrabbableSpawner() = default;

    protected:

        //------------------------//
        //! @name Spawn and react
        //! @{

        //! Called by Graph via a Grabbable when a mouse button is released.
        /*!
         *  @param entity   The entity below.
         *  @param button   The button released.
         *  @param relPos   The position of the Grabbable relative to the entity coordinates.
         *  @param nuiPos   The absolute position of the Grabbable in the window.
         */
        virtual void grabbableButtonReleased(Entity* entity, const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos) {}

        //! Called by Graph via a Grabbable when a mouse button is pressed.
        /*!
         *  @param entity   The entity below.
         *  @param button   The button pressed.
         *  @param relPos   The position of the Grabbable relative to the entity coordinates.
         *  @param nuiPos   The absolute position of the Grabbable in the window.
         */
        virtual void grabbableButtonPressed(Entity* entity, const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos) {}

        //! Called by Graph via a Grabbable when the mouse is moved.
        /*!
         *  @param entity   The entity below.
         *  @param relPos   The position of the Grabbable relative to the entity coordinates.
         *  @param nuiPos   The absolute position of the Grabbable in the window.
         */
        virtual void grabbableMoved(Entity* entity, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos) {}

        //! Creates the temporary grabbable object.
        virtual std::unique_ptr<Grabbable> spawnGrabbable() = 0;

        //! @}
    };

    //! An abstract grabbable object.
    /*!
     *  Each Grabbable needs a spawner to call back when it will be
     *  destroyed. Derived classes specify which Grabbable to spawn (so that
     *  it can have any appearance you want) and decide what happens you
     *  Grabbable is released.
     *
     *  All derived class need to implement the draw() function as
     *  defined in sf::Drawable.
     */

    class Grabbable : public sf::Drawable, public sf::Transformable
    {
    public:

        //! Deleted no-argument constructor.
        Grabbable() = delete;

        //! Constructor given the original spawner.
        Grabbable(GrabbableSpawner& spawner) : m_spawner(spawner) {}

        //! Default destructor.
        virtual ~Grabbable() = default;

        //-----------------//
        //! @name Callback
        //! @{

        //! The spawner from which the grabbable was created.
        inline GrabbableSpawner& spawner() { return m_spawner; }

        //! @}

        //--------------//
        //! @name Hints
        //! @{

        //! An indicator to set the size of the derived grabbable.
        inline const sf::Vector2f& sizeHint() const { return m_sizeHint; }

        //! @}

    private:

        //! The spawner from which the grabbable was created.
        GrabbableSpawner& m_spawner;

        //! An indicator to set the size of the derived grabbable.
        sf::Vector2f m_sizeHint = sf::Vector2f{25.f, 25.f};
    };
}
