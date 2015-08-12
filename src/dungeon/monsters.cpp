#include "dungeon/monsters.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "tools/vector.hpp"

using namespace dungeon;

//-----------------------//
//----- MonsterCage -----//

MonsterCage::MonsterCage(std::wstring monsterID)
    : m_monsterID(std::move(monsterID))
{
    // Background
    addPart(&m_background);
    m_background.setTexture(&Application::context().textures.get(TextureID::DUNGEON_SIDEBAR_TAB_MONSTERS_CAGE));
}

void MonsterCage::onSizeChanges()
{
    // Background, fit texture to height
    m_background.setSize(size());
    const auto& textureSize = m_background.getTexture()->getSize();
    auto textureRectWidth  = static_cast<int>(size().x * (textureSize.y / size().y));
    auto textureRectHeight = static_cast<int>(textureSize.y);
    m_background.setTextureRect({0, 0, textureRectWidth, textureRectHeight});
}

void MonsterCage::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
}

void MonsterCage::grabbableButtonReleased(Entity* entity, const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos)
{
}

std::unique_ptr<scene::Grabbable> MonsterCage::spawnGrabbable()
{
    return std::make_unique<MonsterGrabbable>(*this);
}

//----------------------------//
//----- MonsterGrabbable -----//

MonsterGrabbable::MonsterGrabbable(scene::GrabbableSpawner& spawner)
    : baseClass(spawner)
{
}

void MonsterGrabbable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_sprite, states);
}
