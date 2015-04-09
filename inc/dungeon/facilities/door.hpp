#pragma once

#include "nui/grabbutton.hpp"
#include "scene/grabbable.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace dungeon
{
namespace facilities
{
    //! A DoorGrabbable spawner.
    /*!
     *  Implements the scene::GrabbableSpawner for the dungeon facility door.
     */

    class DoorGrabButton final : public nui::GrabButton
    {
        using baseClass = nui::GrabButton;

    public:

        //! Constructor, affecting texture and look.
        DoorGrabButton();

        //! Default destructor.
        virtual ~DoorGrabButton() = default;

    protected:

        //------------------------//
        //! @name Spawn and react
        //! @{

        void grabbableReleased(Entity* entity, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos) final;
        std::unique_ptr<scene::Grabbable> spawnGrabbable() final;

        //! @}
    };

    //! A door temporary object.
    /*!
     *  Implements the scene::Grabbable for the dungeon facility door.
     */

    class DoorGrabbable final : public scene::Grabbable
    {
        using baseClass = scene::Grabbable;

    public:

        //! Constructor, affecting texture and look.
        DoorGrabbable(scene::GrabbableSpawner& spawner);

        //! Default destructor.
        virtual ~DoorGrabbable() = default;

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
}
