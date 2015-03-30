#include "dungeon/facilities/door.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "resources/identifiers.hpp"
#include "tools/platform-fixes.hpp" // make_unique
#include "tools/vector.hpp"
#include "tools/tools.hpp"
#include "dungeon/inter.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

using namespace dungeon::facilities;

//----------------------//
//----- GrabButton -----//

DoorGrabButton::DoorGrabButton()
{
    setVisual(_("Door"), TextureID::DUNGEON_FACILITIES_DOOR);
}

void DoorGrabButton::grabbableReleased(Entity* entity, const sf::Vector2f& relPos, const sf::Vector2f&)
{
    // Forward to dungeon::Inter
    auto dungeonInter = dynamic_cast<dungeon::Inter*>(entity);
    returnif (dungeonInter == nullptr);
    dungeonInter->constructDoor(relPos);
}

std::unique_ptr<scene::Grabbable> DoorGrabButton::spawnGrabbable()
{
    return std::make_unique<DoorGrabbable>(*this);
}

//---------------------//
//----- Grabbable -----//

DoorGrabbable::DoorGrabbable(scene::GrabbableSpawner& spawner)
    : baseClass(spawner)
{
    m_sprite.setTexture(&Application::context().textures.get(TextureID::DUNGEON_FACILITIES_DOOR));
    m_sprite.setSize(sizeHint());

    setOrigin(sizeHint() / 2.f);
}

void DoorGrabbable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_sprite, states);
}
