#pragma once

#include "nui/grabbutton.hpp"
#include "scene/grabbable.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace dungeon
{
namespace facilities
{
    //----------------------------//
    //----- LadderGrabButton -----//

	class LadderGrabButton : public nui::GrabButton
	{
        using baseClass = nui::GrabButton;

	public:
        LadderGrabButton();
        virtual ~LadderGrabButton() = default;

        void grabbableReleased(Entity* entity, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos) final;
        std::unique_ptr<scene::Grabbable> spawnGrabbable() final;
	};

    //---------------------------//
    //----- LadderGrabbable -----//

	class LadderGrabbable : public scene::Grabbable
	{
        using baseClass = scene::Grabbable;

	public:
        LadderGrabbable(scene::GrabbableSpawner& spawner);
        virtual ~LadderGrabbable() = default;

    // Routine
    void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

    private:
        sf::Sprite m_sprite;
	};
}
}
