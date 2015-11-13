#include "dungeon/sidebar/facilitybutton.hpp"

#include "core/application.hpp"
#include "dungeon/inter.hpp"
#include "tools/platform-fixes.hpp" // make_unique

#include <SFML/Graphics/RenderTarget.hpp>

using namespace dungeon;

//----------------------//
//----- GrabButton -----//

FacilityGrabButton::FacilityGrabButton(const std::wstring& text, std::wstring facilityID)
    : m_facilityID(std::move(facilityID))
{
    m_textureID = "dungeon/facilities/" + toString(m_facilityID) + "/icon";
    setVisual(text, m_textureID);
}

void FacilityGrabButton::grabbableButtonPressed(Entity* entity, const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f&)
{
    // If right button, deselect
    if (button == sf::Mouse::Right) {
        graph()->removeGrabbable();
        return;
    }

    // Forward to dungeon::Inter if it is below
    auto inter = dynamic_cast<Inter*>(entity);
    returnif (inter == nullptr);
    inter->createRoomFacility(relPos, m_facilityID);
}

std::unique_ptr<scene::Grabbable> FacilityGrabButton::spawnGrabbable()
{
    return std::make_unique<FacilityGrabbable>(*this, m_textureID);
}

//---------------------//
//----- Grabbable -----//

FacilityGrabbable::FacilityGrabbable(scene::GrabbableSpawner& spawner, const std::string& textureID)
    : baseClass(spawner)
{
    addPart(&m_sprite);
    m_sprite.setTexture(&Application::context().textures.get(textureID));
    m_sprite.setSize(sizeHint());
    m_sprite.setOrigin(sizeHint() / 2.f);
}
