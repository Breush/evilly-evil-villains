#pragma once

#include "nui/grabbutton.hpp"
#include "scene/grabbable.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace dungeon
{
namespace facilities
{
    //! A LadderGrabbable spawner.
    /*!
     *  Implements the scene::GrabbableSpawner for the dungeon facility ladder.
     */

	class LadderGrabButton final : public nui::GrabButton
	{
        using baseClass = nui::GrabButton;

	public:

        //! Constructor, affecting texture and look.
        LadderGrabButton();

        //! Default destructor.
        virtual ~LadderGrabButton() = default;

    protected:

        //------------------------//
        //! @name Spawn and react
        //! @{

        void grabbableReleased(Entity* entity, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos) final;
        std::unique_ptr<scene::Grabbable> spawnGrabbable() final;

        //! @}
	};

    //! A ladder temporary object.
    /*!
     *  Implements the scene::Grabbable for the dungeon facility ladder.
     */

	class LadderGrabbable final : public scene::Grabbable
	{
        using baseClass = scene::Grabbable;

	public:

        //! Constructor, affecting texture and look.
        LadderGrabbable(scene::GrabbableSpawner& spawner);

        //! Default destructor.
        virtual ~LadderGrabbable() = default;

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

        //! @}

    private:

        //! Holding the image of the grabbable.
        sf::Sprite m_sprite;
	};
}
}
