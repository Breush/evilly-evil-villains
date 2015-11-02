#include "dungeon/monstercage.hpp"

#include "core/application.hpp"
#include "context/villains.hpp"
#include "dungeon/inter.hpp"
#include "dungeon/data.hpp"
#include "tools/vector.hpp"
#include "tools/random.hpp"

using namespace dungeon;

MonsterCage::MonsterCage(std::wstring monsterID, Inter& inter, Data& data)
    : m_inter(inter)
    , m_data(data)
    , m_monsterID(std::move(monsterID))
{
    const auto& monsterData = m_data.monstersDB().get(m_monsterID);

    // Events
    setEmitter(&data);

    // Background
    addPart(&m_reserveBackground);
    m_reserveBackground.setTexture(&Application::context().textures.get("dungeon/sidebar/tab/monsters/cage"));

    // Box name
    attachChild(m_nameLabel);
    m_nameLabel.setDepth(-1.f);
    m_nameLabel.setPrestyle(scene::Label::Prestyle::NUI_SMALL_TITLE);
    m_nameLabel.setText(monsterData.name);
    m_nameLabel.setRelativePosition({0.85f, 0.f});
    m_nameLabel.setRelativeOrigin({0.5f, 0.f});

    // Cost box
    attachChild(m_baseCostLabel);
    m_baseCostLabel.setDepth(-1.f);
    m_baseCostLabel.setPrestyle(scene::Label::Prestyle::NUI_SMALL);
    m_baseCostLabel.setText(toWString(monsterData.baseCost.dosh));

    refreshCostLabelsColor();
    refreshReservePuppetsCount();
}

//-------------------//
//----- Routine -----//

void MonsterCage::onSizeChanges()
{
    // Background, fit texture to height
    m_reserveBackground.setSize({0.7f * size().x, size().y});
    const auto& textureSize = m_reserveBackground.getTexture()->getSize();
    m_scaleFactor = size().y / textureSize.y;

    auto textureRectWidth  = static_cast<int>(m_reserveBackground.getSize().x / m_scaleFactor);
    auto textureRectHeight = static_cast<int>(textureSize.y);
    m_reserveBackground.setTextureRect({0, 0, textureRectWidth, textureRectHeight});

    // Reserve
    refreshReservePuppetsParameters();
}

void MonsterCage::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    // We need to update the color as the label prestyle might override it
    refreshCostLabelsColor();
}

//------------------//
//----- Events -----//

void MonsterCage::receive(const context::Event& event)
{
    const auto& devent = *reinterpret_cast<const dungeon::Event*>(&event);

    if (devent.type == "dosh_changed")
        refreshCostLabelsColor();
    else if (devent.type == "monster_added" && devent.monster.id == m_monsterID)
        refreshReservePuppetsCount();
}

bool MonsterCage::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    returnif (button != sf::Mouse::Left) false;
    graph()->setGrabbable(spawnGrabbable());
    return true;
}

//---------------------//
//----- Grabbable -----//

void MonsterCage::grabbableMoved(Entity* entity, const sf::Vector2f& relPos, const sf::Vector2f&)
{
    returnif (entity != &m_inter);
    m_inter.setPredictionMonster(relPos, m_monsterID);
}

void MonsterCage::grabbableButtonReleased(Entity* entity, const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left);
    m_puppets.back()->setVisible(true);
    graph()->removeGrabbable();
    m_inter.resetPrediction();

    returnif (entity != &m_inter);
    m_inter.moveMonsterFromReserve(relPos, m_monsterID);
}

std::unique_ptr<scene::Grabbable> MonsterCage::spawnGrabbable()
{
    // Grab if puppets available
    returnif (m_puppets.size() == 0u) nullptr;

    // And hide one puppet, as it is being grabbed
    m_puppets.back()->setVisible(false);
    return std::make_unique<MonsterGrabbable>(*this, m_monsterID, 0.5f * m_inter.roomScale());
}

//-----------------------------------//
//----- Internal change updates -----//

void MonsterCage::refreshReservePuppetsCount()
{
    const auto& reserve = m_data.monstersInfo().reserve;
    auto pCage = std::find_if(reserve, [this] (const Data::MonsterCageInfo& monsterCage) { return monsterCage.type == m_monsterID; });
    uint monstersCount = (pCage == std::end(reserve))? 0u : pCage->monsters.size();
    uint monstersCountBefore = m_puppets.size();

    returnif (monstersCountBefore == monstersCount);

    // Resize and configure new ones
    m_puppets.resize(monstersCount);
    for (uint i = monstersCountBefore; i < monstersCount; ++i) {
        auto& puppet = m_puppets.at(i);
        puppet = std::make_unique<ai::DumbPuppet>();
        puppet->setSource(toString(L"dungeon/monsters/" + m_monsterID));
        attachChild(*puppet);
    }

    // We're in this function because the number of monster changed,
    // just update the parameters of the new ones
    refreshReservePuppetsParameters(monstersCountBefore);
}

void MonsterCage::refreshReservePuppetsParameters(const uint monstersUpdateStart)
{
    const auto& monsterData = m_data.monstersDB().get(m_monsterID);
    const auto& reserveSize = m_reserveBackground.getSize();

    for (uint i = monstersUpdateStart; i < m_puppets.size(); ++i) {
        auto& puppet = m_puppets.at(i);
        puppet->lerpable()->setPositionSpeed(monsterData.speed * m_inter.tileSize());
        puppet->setLocalScale({m_scaleFactor, m_scaleFactor});

        // TODO Why 25.f? Use collision box, position 0.f means 0.f + colBox
        float startOffset = alea::rand(25.f, reserveSize.x - 25.f);
        puppet->setInitialLocalPosition({startOffset, 0.5f * reserveSize.y});
        puppet->setHorizontalRange(25.f, reserveSize.x - 25.f);
    }
}

void MonsterCage::refreshCostLabelsColor()
{
    const auto& monsterData = m_data.monstersDB().get(m_monsterID);
    const auto& heldDosh = m_data.villain().doshWallet.value();

    m_baseCostLabel.setColor((heldDosh >= monsterData.baseCost.dosh)? sf::Color::White : sf::Color::Red);
}

//----------------------------//
//----- MonsterGrabbable -----//

MonsterGrabbable::MonsterGrabbable(scene::GrabbableSpawner& spawner, const std::wstring& monsterID, const sf::Vector2f& scale)
    : baseClass(spawner)
{
    m_sprite.load(toString(L"dungeon/monsters/" + monsterID));
    m_sprite.setLocalScale(scale);
}

void MonsterGrabbable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // scene::AnimatedSprite is not supposed to take states.transform in consideration, and does not
    const_cast<scene::AnimatedSprite&>(m_sprite).setLocalPosition(getPosition());
    target.draw(m_sprite, states);
}
