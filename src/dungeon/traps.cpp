#include "dungeon/traps.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "dungeon/inter.hpp"
#include "tools/platform-fixes.hpp" // make_unique

#include <SFML/Graphics/RenderTarget.hpp>

using namespace dungeon;

//----------------------//
//----- GrabButton -----//

TrapGrabButton::TrapGrabButton(const std::wstring& text, TextureID textureID, std::wstring trapID)
    : m_textureID(textureID)
    , m_trapID(std::move(trapID))
{
    setVisual(text, textureID);
}

void TrapGrabButton::grabbableReleased(Entity* entity, const sf::Vector2f& relPos, const sf::Vector2f&)
{
    // Forward to dungeon::Inter if it is below
    auto dungeonInter = dynamic_cast<dungeon::Inter*>(entity);
    returnif (dungeonInter == nullptr);
    dungeonInter->setRoomTrap(relPos, m_trapID);
}

std::unique_ptr<scene::Grabbable> TrapGrabButton::spawnGrabbable()
{
    return std::make_unique<TrapGrabbable>(*this, m_textureID);
}

//---------------------//
//----- Grabbable -----//

TrapGrabbable::TrapGrabbable(scene::GrabbableSpawner& spawner, TextureID textureID)
    : baseClass(spawner)
{
    m_sprite.setTexture(&Application::context().textures.get(textureID));
    m_sprite.setSize(sizeHint());

    setOrigin(sizeHint() / 2.f);
}

void TrapGrabbable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_sprite, states);
}
