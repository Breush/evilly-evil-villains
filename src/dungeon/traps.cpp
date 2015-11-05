#include "dungeon/traps.hpp"

#include "core/application.hpp"
#include "dungeon/inter.hpp"
#include "tools/platform-fixes.hpp" // make_unique
#include "tools/string.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

using namespace dungeon;

//----------------------//
//----- GrabButton -----//

TrapGrabButton::TrapGrabButton(const std::wstring& text, std::wstring trapID)
    : m_trapID(std::move(trapID))
{
    setVisual(text, toString(L"dungeon/traps/" + m_trapID + L"/icon"));
}

void TrapGrabButton::grabbableButtonReleased(Entity* entity, const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left);
    graph()->removeGrabbable();

    // Forward to dungeon::Inter if it is below
    auto inter = dynamic_cast<Inter*>(entity);
    returnif (inter == nullptr);
    inter->setRoomTrap(relPos, m_trapID);
}

std::unique_ptr<scene::Grabbable> TrapGrabButton::spawnGrabbable()
{
    return std::make_unique<TrapGrabbable>(*this, m_trapID);
}

//---------------------//
//----- Grabbable -----//

TrapGrabbable::TrapGrabbable(scene::GrabbableSpawner& spawner, const std::wstring& trapID)
    : baseClass(spawner)
{
    auto textureID = "dungeon/traps/" + toString(trapID) + "/icon";

    addPart(&m_sprite);
    m_sprite.setTexture(&Application::context().textures.get(textureID));
    m_sprite.setSize(sizeHint());
    m_sprite.setOrigin(sizeHint() / 2.f);
}
