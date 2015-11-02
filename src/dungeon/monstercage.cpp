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
    // Events
    setEmitter(&data);

    // Background
    addPart(&m_background);
    m_background.setTexture(&Application::context().textures.get("dungeon/sidebar/tab/monsters/cage"));

    // Cost box
    attachChild(m_baseCostLabel);
    const auto& monsterData = m_data.monstersDB().get(m_monsterID);
    m_baseCostLabel.setPrestyle(scene::RichLabel::Prestyle::NUI);
    m_baseCostLabel.setText(toWString(monsterData.baseCost.dosh));
    refreshCostLabelsColor();

    refreshReserve();
}

//-------------------//
//----- Routine -----//

void MonsterCage::onSizeChanges()
{
    // Background, fit texture to height
    m_background.setSize(size());
    const auto& textureSize = m_background.getTexture()->getSize();
    m_scaleFactor = size().y / textureSize.y;

    auto textureRectWidth  = static_cast<int>(size().x / m_scaleFactor);
    auto textureRectHeight = static_cast<int>(textureSize.y);
    m_background.setTextureRect({0, 0, textureRectWidth, textureRectHeight});

    // Reserve
    refreshReserve();
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
    returnif (devent.type != "dosh_changed");

    refreshCostLabelsColor();
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
    m_inter.addMonster(relPos, m_monsterID);
}

std::unique_ptr<scene::Grabbable> MonsterCage::spawnGrabbable()
{
    // Grab if puppets available
    returnif (m_puppets.size() == 0u) nullptr;

    // And hide one puppet, as it is being grabbed
    m_puppets.back()->setVisible(false);
    return std::make_unique<MonsterGrabbable>(*this, m_monsterID);
}

//-----------------------------------//
//----- Internal change updates -----//

void MonsterCage::refreshReserve()
{
    const auto& reserve = m_data.monstersInfo().reserve;
    const auto& monsterData = m_data.monstersDB().get(m_monsterID);
    auto pCage = std::find_if(reserve, [this] (const Data::MonsterCageInfo& monsterCage) { return monsterCage.type == m_monsterID; });
    uint nMonsters = (pCage == std::end(reserve))? 0u : pCage->monsters.size();

    // Set the correct number of puppets
    if (m_puppets.size() != nMonsters) {
        // Clear all previous
        m_puppets.clear();

        // Attach new ones
        for (uint i = 0u; i < nMonsters; ++i) {
            auto puppet = std::make_unique<ai::DumbPuppet>();
            puppet->setSource(toString(L"dungeon/monsters/" + m_monsterID));
            m_puppets.emplace_back(std::move(puppet));
            attachChild(*m_puppets.back());
        }
    }

    // Re-configure them
    for (auto& puppet : m_puppets) {
        puppet->lerpable()->setPositionSpeed(monsterData.speed * m_inter.tileSize());
        puppet->setLocalScale({m_scaleFactor, m_scaleFactor});

        // TODO Why 25.f? Use collision box, position 0.f means 0.f + colBox
        float startOffset = alea::rand(25.f, size().x - 25.f);
        puppet->setInitialLocalPosition({startOffset, 0.5f * size().y});
        puppet->setHorizontalRange(25.f, size().x - 25.f);
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

MonsterGrabbable::MonsterGrabbable(scene::GrabbableSpawner& spawner, const std::wstring& monsterID)
    : baseClass(spawner)
{
    m_sprite.load(toString(L"dungeon/monsters/" + monsterID));
    m_sprite.setLocalScale({0.5f, 0.5f});
}

void MonsterGrabbable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // scene::AnimatedSprite is not supposed to take states.transform in consideration, and does not
    const_cast<scene::AnimatedSprite&>(m_sprite).setLocalPosition(getPosition());
    target.draw(m_sprite, states);
}
