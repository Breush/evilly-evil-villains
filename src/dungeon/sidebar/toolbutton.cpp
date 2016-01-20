#include "dungeon/sidebar/toolbutton.hpp"

#include "core/application.hpp"
#include "dungeon/inter.hpp"
#include "scene/graph.hpp"
#include "tools/platform-fixes.hpp" // make_unique

#include <SFML/Graphics/RenderTarget.hpp>

using namespace dungeon;

//----------------------//
//----- GrabButton -----//

ToolGrabButton::ToolGrabButton(const std::wstring& text, ToolID toolID)
    : m_toolID(toolID)
{
    switch (m_toolID) {
        case ToolID::ROOMS_BUILDER:
            m_imageTextureID = "core/tools/rooms_builder_icon";
            break;

        case ToolID::ROOMS_DESTROYER:
            m_imageTextureID = "core/tools/rooms_destroyer_icon";
            break;

        case ToolID::TRAPS_REMOVER:
            m_imageTextureID = "core/tools/traps_remover_icon";
            break;

        case ToolID::FACILITIES_REMOVER:
            m_imageTextureID = "core/tools/facilities_remover_icon";
            break;
    }

    set(text, m_imageTextureID);
}

void ToolGrabButton::grabbableButtonPressed(scene::Entity* entity, const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f&)
{
    // If right button, deselect
    if (button == sf::Mouse::Right) {
        graph()->removeGrabbable();
        return;
    }

    // Forward to dungeon::Inter if it is below
    sf::Vector2f interRelPos(relPos);
    returnif (entity == nullptr);
    auto inter = entity->findParent<Inter>(interRelPos);
    returnif (inter == nullptr);

    // Different action given the toolID
    switch (m_toolID) {
        case ToolID::ROOMS_BUILDER:
            inter->constructRoom(interRelPos);
            break;

        case ToolID::ROOMS_DESTROYER:
            inter->destroyRoom(interRelPos);
            break;

        case ToolID::TRAPS_REMOVER:
            inter->removeRoomTrap(interRelPos);
            break;

        case ToolID::FACILITIES_REMOVER:
            inter->removeRoomFacilities(interRelPos);
            break;
    }
}

std::unique_ptr<scene::Grabbable> ToolGrabButton::spawnGrabbable()
{
    return std::make_unique<ToolGrabbable>(*this, m_imageTextureID);
}

//---------------------//
//----- Grabbable -----//

ToolGrabbable::ToolGrabbable(scene::GrabbableSpawner& spawner, const std::string& textureID)
    : baseClass(spawner)
{
    addPart(&m_sprite);
    m_sprite.setTexture(&Application::context().textures.get(textureID));
    m_sprite.setSize(sizeHint());
    m_sprite.setOrigin(sizeHint() / 2.f);
}
