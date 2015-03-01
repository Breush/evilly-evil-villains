#include "dungeon/facilities/ladder.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "resources/identifiers.hpp"
#include "tools/platform-fixes.hpp" // make_unique
#include "dungeon/inter.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

using namespace dungeon::facilities;

//----------------------------//
//----- LadderGrabButton -----//

LadderGrabButton::LadderGrabButton()
{
    // TODO Get tabImageSize from somewhere or directly stored in imageButton?
    sf::Vector2f tabImageSize(80.f, 80.f);

    setVisual(_("Ladder"), TextureID::DUNGEON_FACILITIES_LADDER, tabImageSize);
}

void LadderGrabButton::grabbableReleased(Entity* entity, const sf::Vector2f& relPos, const sf::Vector2f&)
{
    // Forward to dungeon::Inter
    auto dungeonInter = dynamic_cast<dungeon::Inter*>(entity);
    returnif (dungeonInter == nullptr);
    dungeonInter->constructLadder(relPos);
}

std::unique_ptr<scene::Grabbable> LadderGrabButton::spawnGrabbable()
{
    return std::make_unique<LadderGrabbable>(*this);
}

//---------------------------------//
//----- LadderGrabbableEntity -----//

LadderGrabbable::LadderGrabbable(scene::GrabbableSpawner& spawner)
    : baseClass(spawner)
{
    // TODO Here too: get size from somewhere
    sf::Vector2f tabImageSize(80.f, 80.f);

    const auto& texture = Application::context().textures.get(TextureID::DUNGEON_FACILITIES_LADDER);
    auto textureSize = sf::v2f(texture.getSize());
    m_sprite.setTexture(texture);
    m_sprite.setScale(sf::vdiv(tabImageSize, textureSize));

    setOrigin(tabImageSize / 2.f);
}

void LadderGrabbable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_sprite, states);
}
