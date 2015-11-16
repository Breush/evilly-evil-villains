#include "dungeon/sidebar/facilitybutton.hpp"

#include "core/application.hpp"
#include "dungeon/inter.hpp"
#include "tools/platform-fixes.hpp" // make_unique

#include <SFML/Graphics/RenderTarget.hpp>

using namespace dungeon;

//----------------------//
//----- GrabButton -----//

FacilityGrabButton::FacilityGrabButton(const FacilitiesDB::FacilityData& facilityData, std::wstring facilityID)
    : m_facilityData(facilityData)
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

void FacilityGrabButton::grabbableButtonPressed(Entity* entity, const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f&)
{
    // If right button, deselect
    if (button == sf::Mouse::Right) {
        // Still some explicit links to do, do not stop construction
        returnif (m_explicitLinksDone != 0u && m_explicitLinksDone < m_explicitLinksCount);
        m_grabbableFacilityID = m_facilityID;
        graph()->removeGrabbable();
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
    bool created = inter->createRoomFacility(coords, m_grabbableFacilityID);
    returnif (m_explicitLinksCount == 0u);
    returnif (!created);

    // FIXME If we were creating an explicit link,
    // but the main has been destroyed since,
    // we create an invalid pending link

    // If we just created the main of explicit links, remember that
    if (m_explicitLinksDone == 0u) {
        m_explicitLinkCoords = coords;
    }
    // If we're created a link, link them together
    else {
        inter->setRoomFacilityLink(m_explicitLinkCoords, m_facilityID, coords);
        inter->setRoomFacilityLink(coords, m_grabbableFacilityID, m_explicitLinkCoords);
    }

    // Find and use next link info
    if (m_explicitLinksDone < m_explicitLinksCount) {
        // Find next explicit link
        const Link* pLink = nullptr;
        uint explicitLinkCount = 0u;
        for (const auto& link : m_facilityData.links)
            if (link.style == Link::Style::EXPLICIT)
                if (explicitLinkCount++ == m_explicitLinksDone)
                    pLink = &link;

        // Display of the next link
        setGrabbableFacilityID(pLink->id);
        ++m_explicitLinksDone;
    }

    // If we did enough links, go back to the main
    else if (m_explicitLinksDone == m_explicitLinksCount) {
        m_explicitLinksDone = 0u;
        setGrabbableFacilityID(m_facilityID);
    }
}

std::unique_ptr<scene::Grabbable> FacilityGrabButton::spawnGrabbable()
{
    m_explicitLinksDone = 0u;
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
