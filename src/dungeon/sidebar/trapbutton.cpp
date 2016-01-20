#include "dungeon/sidebar/trapbutton.hpp"

#include "core/application.hpp"
#include "dungeon/inter.hpp"
#include "scene/graph.hpp"
#include "tools/platform-fixes.hpp" // make_unique
#include "tools/string.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

using namespace dungeon;

//----------------------//
//----- GrabButton -----//

TrapGrabButton::TrapGrabButton(const std::wstring& text, std::wstring trapID)
    : m_trapID(std::move(trapID))
{
    set(text, toString(L"vanilla/traps/" + m_trapID + L"/icon"));
}

void TrapGrabButton::grabbableButtonReleased(scene::Entity* entity, const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left);
    graph()->removeGrabbable();

    // Forward to dungeon::Inter if it is below
    sf::Vector2f interRelPos(relPos);
    returnif (entity == nullptr);
    auto inter = entity->findParent<Inter>(interRelPos);
    returnif (inter == nullptr);
    inter->setRoomTrap(interRelPos, m_trapID);
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
