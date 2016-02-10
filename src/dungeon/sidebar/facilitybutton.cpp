#include "dungeon/sidebar/facilitybutton.hpp"

#include "context/context.hpp"
#include "dungeon/inter.hpp"
#include "scene/graph.hpp"
#include "tools/platform-fixes.hpp" // make_unique

#include <SFML/Graphics/RenderTarget.hpp>

using namespace dungeon;

//----------------------//
//----- GrabButton -----//

FacilityGrabButton::FacilityGrabButton(const FacilityData& facilityData, std::wstring facilityID, Inter& inter)
    : m_inter(inter)
    , m_facilityData(facilityData)
    , m_facilityID(std::move(facilityID))
{
    m_grabbableFacilityID = m_facilityID;
    m_imageTextureID = "vanilla/facilities/" + toString(m_facilityID) + "/icon";
    set(m_facilityData.name, m_imageTextureID);

    m_interactiveLinksCount = m_facilityData.interactiveLinks.size();
}

void FacilityGrabButton::grabbableMoved(scene::Entity* entity, const sf::Vector2f& relPos, const sf::Vector2f&)
{
    // Show the facility to create if inter is below
    sf::Vector2f interRelPos(relPos);
    returnif (entity == nullptr);
    auto inter = entity->findParent<Inter>(interRelPos);

    if (inter == nullptr) {
        m_inter.resetPrediction();
        m_inter.resetPredictionLink();
        return;
    }

    m_inter.setPredictionFacility(interRelPos, m_facilityID);

    // Show link if linking
    returnif (m_interactiveLinksCount == 0u || m_interactiveLinksDone == 0u);
    auto targetCoords = inter->roomCoordsFromPosition(interRelPos);
    inter->setPredictionLink(m_originalLinkCoords, targetCoords);
}

void FacilityGrabButton::grabbableButtonPressed(scene::Entity* entity, const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f&)
{
    // If right button, deselect
    if (button == sf::Mouse::Right) {
        m_grabbableFacilityID = m_facilityID;
        m_inter.resetPrediction();
        m_inter.resetPredictionLink();
        graph()->removeGrabbable();
        m_interactiveLinksDone = 0u;
        return;
    }

    // Forward to dungeon::Inter if it is below
    sf::Vector2f interRelPos(relPos);
    returnif (entity == nullptr);
    auto inter = entity->findParent<Inter>(interRelPos);
    returnif (inter == nullptr);

    // TODO We might want link patterns:
    //      - All linked to the first one (star)
    //      - Linked as a chain (possibliy a circle)
    auto coords = m_inter.roomCoordsFromPosition(interRelPos);
    if (m_interactiveLinksDone == 0u) {
        m_originalLinkCoords = coords;
        returnif (!m_inter.facilitiesCreate(coords, m_grabbableFacilityID));
    }
    else {
        // Checking constraints
        const auto& link = m_facilityData.interactiveLinks[m_interactiveLinksDone - 1u];
        returnif (constraintsExclude(link.constraints, coords - m_originalLinkCoords));

        // Create the associated facility (if any)
        if (!link.facilityID.empty())
            returnif (!m_inter.facilitiesCreate(coords, link.facilityID));

        // Simply create the link
        m_inter.facilityLinksInteractiveCreate(m_originalLinkCoords, m_facilityID, &link, coords);
    }

    returnif (m_interactiveLinksCount == 0u);

    // Find and use next link info
    if (m_interactiveLinksDone < m_interactiveLinksCount) {
        const auto& link = m_facilityData.interactiveLinks[m_interactiveLinksDone];
        if (!link.facilityID.empty())
            setGrabbableFacilityID(link.facilityID);
        ++m_interactiveLinksDone;
    }

    // If we did enough links, go back to the main
    else if (m_interactiveLinksDone == m_interactiveLinksCount) {
        m_interactiveLinksDone = 0u;
        inter->resetPredictionLink();
        setGrabbableFacilityID(m_facilityID);
    }
}

std::unique_ptr<scene::Grabbable> FacilityGrabButton::spawnGrabbable()
{
    return std::make_unique<FacilityGrabbable>(*this, m_imageTextureID);
}

void FacilityGrabButton::setGrabbableFacilityID(const std::wstring& facilityID)
{
    m_grabbableFacilityID = facilityID;
    m_imageTextureID = "dungeon/facilities/" + toString(m_grabbableFacilityID) + "/icon";
    graph()->setGrabbable(spawnGrabbable());
}

//---------------------//
//----- Grabbable -----//

FacilityGrabbable::FacilityGrabbable(scene::GrabbableSpawner& spawner, const std::string& textureID)
    : baseClass(spawner)
{
    addPart(&m_sprite);
    m_sprite.setTexture(&context::context.textures.get(textureID));
    m_sprite.setSize(sizeHint());
    m_sprite.setOrigin(sizeHint() / 2.f);
}
