#include "dungeon/facilities/ladder.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "resources/identifiers.hpp"
#include "tools/platform-fixes.hpp" // make_unique
#include "tools/vector.hpp"
#include "tools/tools.hpp"
#include "dungeon/inter.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

using namespace dungeon::facilities;

//----------------------------//
//----- LadderGrabButton -----//

LadderGrabButton::LadderGrabButton()
{
    setVisual(_("Ladder"), TextureID::DUNGEON_FACILITIES_LADDER);
}

void LadderGrabButton::grabbableReleased(Entity* entity, const sf::Vector2f& relPos, const sf::Vector2f&)
{
    // Forward to dungeon::Inter
    auto dungeonInter = dynamic_cast<dungeon::Inter*>(entity);
    returnif (dungeonInter == nullptr);
    dungeonInter->constructDoor(relPos);
}

std::unique_ptr<scene::Grabbable> LadderGrabButton::spawnGrabbable()
{
    return std::make_unique<LadderGrabbable>(*this);
}

//---------------------------//
//----- LadderGrabbable -----//

LadderGrabbable::LadderGrabbable(scene::GrabbableSpawner& spawner)
    : baseClass(spawner)
{
    m_sprite.setTexture(&Application::context().textures.get(TextureID::DUNGEON_FACILITIES_LADDER));
    m_sprite.setSize(sizeHint());

    setOrigin(sizeHint() / 2.f);
}

void LadderGrabbable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_sprite, states);
}
