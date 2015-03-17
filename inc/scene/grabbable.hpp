#pragma once

#include "config/nui.hpp"

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

        //! Called by Graph via a Grabbable when the latter is released and before it's destroyed.
        /*!
         *  @param entity   The entity below the Grabbable when released.
         *  @param relPos   The position of the Grabbable relative to the entity coordinates.
         *  @param nuiPos   The absolute position of the Grabbable in the window.
         */
        virtual void grabbableReleased(Entity* entity, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos) = 0;

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
        Grabbable(GrabbableSpawner& spawner) : m_spawner(spawner)
        {
            refreshDisplay();
        }

        //! Default destructor.
        virtual ~Grabbable() = default;

        //----------------//
        //! @name Routine
        //! @{

        //! To be called whenever the config NUI changed (managed by scene::Graph).
        inline void refreshDisplay()
        {
            config::NUI cNUI;
            m_sizeHint = {cNUI.hintImageSide, cNUI.hintImageSide};
        }

        //! @}

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
        sf::Vector2f m_sizeHint;
    };
}
