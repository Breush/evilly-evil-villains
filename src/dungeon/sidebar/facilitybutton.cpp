#include "dungeon/sidebar/facilitybutton.hpp"

#include "core/application.hpp"
#include "dungeon/inter.hpp"
#include "tools/platform-fixes.hpp" // make_unique

#include <SFML/Graphics/RenderTarget.hpp>

using namespace dungeon;

//----------------------//
//----- GrabButton -----//

FacilityGrabButton::FacilityGrabButton(const FacilitiesDB::FacilityData& facilityData, std::wstring facilityID, Inter& inter)
    : m_inter(inter)
    , m_facilityData(facilityData)
    , m_facilityID(std::move(facilityID))
{
    m_grabbableFacilityID = m_facilityID;
    m_textureID = "dungeon/facilities/" + toString(m_facilityID) + "/icon";
    setVisual(m_facilityData.name, m_textureID);

    // Explicit links (m_explicitLinksCount is constructed to 0u)
    for (const auto& link : m_facilityData.links)
        if (link.style == Link::Style::EXPLICIT)
            ++m_explicitLinksCount;
}

void FacilityGrabButton::grabbableMoved(Entity* entity, const sf::Vector2f& relPos, const sf::Vector2f&)
{
    // Show link if linking
    returnif (m_explicitLinksCount == 0u || m_explicitLinksDone == 0u);

    // Forward to dungeon::Inter if it is below
    auto inter = dynamic_cast<Inter*>(entity);
    returnif (inter == nullptr);

    auto targetCoords = inter->tileFromLocalPosition(relPos);
    inter->setPredictionLink(m_explicitLinkCoords, targetCoords);
}

void FacilityGrabButton::grabbableButtonPressed(Entity* entity, const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f&)
{
    // If right button, deselect
    if (button == sf::Mouse::Right) {
        m_grabbableFacilityID = m_facilityID;
        m_inter.resetPredictionLink();
        graph()->removeGrabbable();
        m_explicitLinksDone = 0u;
        return;
    }

    // Forward to dungeon::Inter if it is below
    auto inter = dynamic_cast<Inter*>(entity);
    returnif (inter == nullptr);

    // TODO In fact, facility does only remember one link,
    // hence that multiple links thingy is useless for now.
    // The first room will only get the last one as a link.
    // + We might want link patterns:
    //  - All linked to the first one
    //  - Linked as a chain (possibliy a circle)
    auto coords = inter->tileFromLocalPosition(relPos);
    if (m_explicitLinksDone == 0u) {
        m_explicitLinkCoords = coords;
        returnif (!inter->createRoomFacility(coords, m_grabbableFacilityID));
    }
    else {
        // Checking constraints
        const auto& link = getCurrentExplicitLink();
        for (const auto& constraint : link.constraints) {
            if (constraint.mode == Constraint::Mode::EXCLUDE) {
                if (constraint.x.type == ConstraintParameter::Type::EQUAL)
                    returnif (m_explicitLinkCoords.x + constraint.x.value == coords.x);
                if (constraint.y.type == ConstraintParameter::Type::EQUAL)
                    returnif (m_explicitLinkCoords.y + constraint.y.value == coords.y);
            }
        }

        // If all constraints are OK, create the facility
        returnif (!inter->createRoomFacilityLinked(coords, m_grabbableFacilityID, m_explicitLinkCoords, m_facilityID));
    }

    returnif (m_explicitLinksCount == 0u);

    // Find and use next link info
    if (m_explicitLinksDone < m_explicitLinksCount) {
        ++m_explicitLinksDone;
        const auto& link = getCurrentExplicitLink();
        setGrabbableFacilityID(link.id);
    }

    // If we did enough links, go back to the main
    else if (m_explicitLinksDone == m_explicitLinksCount) {
        m_explicitLinksDone = 0u;
        inter->resetPredictionLink();
        setGrabbableFacilityID(m_facilityID);
    }
}

const Link& FacilityGrabButton::getCurrentExplicitLink() const
{
    const Link* pLink = nullptr;
    uint explicitLinkCount = 0u;

    for (const auto& link : m_facilityData.links)
        if (link.style == Link::Style::EXPLICIT)
            if (++explicitLinkCount == m_explicitLinksDone)
                pLink = &link;

    return *pLink;
}

std::unique_ptr<scene::Grabbable> FacilityGrabButton::spawnGrabbable()
{
    return std::make_unique<FacilityGrabbable>(*this, m_textureID);
}

void FacilityGrabButton::setGrabbableFacilityID(const std::wstring& facilityID)
{
    m_grabbableFacilityID = facilityID;
    m_textureID = "dungeon/facilities/" + toString(m_grabbableFacilityID) + "/icon";
    graph()->setGrabbable(spawnGrabbable());
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
