#include "dungeon/tools.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "dungeon/inter.hpp"
#include "tools/platform-fixes.hpp" // make_unique

#include <SFML/Graphics/RenderTarget.hpp>

using namespace dungeon;

//----------------------//
//----- GrabButton -----//

ToolGrabButton::ToolGrabButton(const std::wstring& text, ToolID toolID)
    : m_toolID(toolID)
{
    switch (m_toolID) {
        case ToolID::ROOMS_DESTROYER:
            m_textureID = TextureID::DUNGEON_TOOLS_ROOMS_DESTROYER_ICON;
            break;

        case ToolID::TRAPS_REMOVER:
            m_textureID = TextureID::DUNGEON_TOOLS_TRAPS_REMOVER_ICON;
            break;

        case ToolID::FACILITIES_REMOVER:
            m_textureID = TextureID::DUNGEON_TOOLS_FACILITIES_REMOVER_ICON;
            break;
    }

    setVisual(text, m_textureID);
}

void ToolGrabButton::grabbableButtonPressed(Entity* entity, const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f&)
{
    // If right button, deselect
    if (button == sf::Mouse::Right) {
        graph()->removeGrabbable();
        return;
    }

    // Forward to dungeon::Inter if it is below
    auto dungeonInter = dynamic_cast<dungeon::Inter*>(entity);
    returnif (dungeonInter == nullptr);

    // TODO Remove differently given the toolID
    //dungeonInter->setRoomTrap(relPos, m_trapID);
    switch (m_toolID) {
        case ToolID::ROOMS_DESTROYER:
            dungeonInter->destroyRoom(relPos);
            break;

        case ToolID::TRAPS_REMOVER:
            dungeonInter->removeRoomTrap(relPos);
            break;

        case ToolID::FACILITIES_REMOVER:
            dungeonInter->removeRoomFacilities(relPos);
            break;
    }
}

std::unique_ptr<scene::Grabbable> ToolGrabButton::spawnGrabbable()
{
    return std::make_unique<ToolGrabbable>(*this, m_textureID);
}

//---------------------//
//----- Grabbable -----//

ToolGrabbable::ToolGrabbable(scene::GrabbableSpawner& spawner, TextureID textureID)
    : baseClass(spawner)
{
    m_sprite.setTexture(&Application::context().textures.get(textureID));
    m_sprite.setSize(sizeHint());

    setOrigin(sizeHint() / 2.f);
}

void ToolGrabbable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_sprite, states);
}
