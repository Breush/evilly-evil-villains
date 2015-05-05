#include "dungeon/facilities.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "dungeon/inter.hpp"
#include "tools/platform-fixes.hpp" // make_unique

#include <SFML/Graphics/RenderTarget.hpp>

using namespace dungeon;

//----------------------//
//----- GrabButton -----//

FacilityGrabButton::FacilityGrabButton(const std::wstring& text, TextureID textureID, FacilityID facilityID)
    : m_textureID(textureID)
    , m_facilityID(facilityID)
{
    setVisual(text, textureID);
}

void FacilityGrabButton::grabbableReleased(Entity* entity, const sf::Vector2f& relPos, const sf::Vector2f&)
{
    // Forward to dungeon::Inter if it is below
    auto dungeonInter = dynamic_cast<dungeon::Inter*>(entity);
    returnif (dungeonInter == nullptr);
    dungeonInter->setRoomFacility(relPos, m_facilityID, true);
}

std::unique_ptr<scene::Grabbable> FacilityGrabButton::spawnGrabbable()
{
    return std::make_unique<FacilityGrabbable>(*this, m_textureID);
}

//---------------------//
//----- Grabbable -----//

FacilityGrabbable::FacilityGrabbable(scene::GrabbableSpawner& spawner, TextureID textureID)
    : baseClass(spawner)
{
    m_sprite.setTexture(&Application::context().textures.get(textureID));
    m_sprite.setSize(sizeHint());

    setOrigin(sizeHint() / 2.f);
}

void FacilityGrabbable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_sprite, states);
}