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

    // Cost banner
    attachChild(m_baseCostBanner);
    m_baseCostBanner.setDepth(-1.f);
    m_baseCostBanner.setCost(monsterData.baseCost);
    m_baseCostBanner.setRelativePosition({0.85f, 0.25f});
    m_baseCostBanner.setRelativeOrigin({0.5f, 0.f});

    // Hire box
    attachChild(m_hireBoxBackground);
    m_hireBoxBackground.setOutlineThickness(1.f);
    m_hireBoxBackground.setOutlineColor(sf::Color::White);
    m_hireBoxBackground.setFillColor({0u, 0u, 0u, 185u});
    m_hireBoxBackground.setRelativeOrigin({1.f, 1.f});

    attachChild(m_hireBoxLabel);
    m_hireBoxLabel.setDepth(-1.f);
    m_hireBoxLabel.setPrestyle(scene::RichLabel::Prestyle::NUI_SMALL);
    m_hireBoxLabel.setRelativeOrigin({0.f, 1.f});

    refreshReservePuppetsCount();
}

//-------------------//
//----- Routine -----//

void MonsterCage::onSizeChanges()
{
    // Hire box background, taking outline in account
    m_hireBoxBackground.setLocalPosition(size() - 1.f);

    refreshReservePuppetsParameters();
    refreshHireBox();
}

void MonsterCage::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_padding = 0.3f * (cNUI.hPadding + cNUI.vPadding);

    refreshHireBox();
}

//------------------//
//----- Events -----//

void MonsterCage::receive(const context::Event& event)
{
    const auto& devent = *reinterpret_cast<const dungeon::Event*>(&event);

    if (devent.type == "monster_added" && devent.monster.id == m_monsterID)
        refreshReservePuppetsCount();
}

bool MonsterCage::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    returnif (button != sf::Mouse::Left) false;

    // Cage part
    if (mousePos.x < 0.7f * size().x)
        graph()->setGrabbable(spawnGrabbable());
    // Hire part
    else
        hire();

    return true;
}

//---------------------//
//----- Grabbable -----//

void MonsterCage::grabbableMoved(Entity* entity, const sf::Vector2f& relPos, const sf::Vector2f&)
{
    if (entity != &m_inter) {
        m_inter.resetPrediction();
        return;
    }

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

//------------------//
//----- Hiring -----//

void MonsterCage::hire()
{
    // TODO Check for cost (nor Inter nor Data should manage this)
    // TODO Check against hire time

    m_inter.addMonsterToReserve(m_monsterID);
}

//-----------------------------------//
//----- Internal change updates -----//

void MonsterCage::refreshHireBox()
{
    const auto& reserve = m_data.monstersInfo().reserve;
    auto pCage = std::find_if(reserve, [this] (const Data::MonsterCageInfo& monsterCage) { return monsterCage.type == m_monsterID; });
    returnif (pCage == std::end(reserve));

    // Text
    m_hireBoxLabel.setText(L"*Hire* " + toWString(pCage->countdown));
    m_hireBoxLabel.setLocalPosition({0.7f * size().x + m_padding, -m_padding});

    // Background
    auto hireBoxSize = sf::Vector2f(0.3f * size().x - 2.f, m_hireBoxLabel.size().y + 2.f * m_padding - 2.f);
    m_hireBoxBackground.setSize(hireBoxSize);
}

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
    auto reserveSize = sf::Vector2f(0.7f * size().x, size().y);

    // Background
    m_reserveBackground.setSize(reserveSize);
    const auto& textureSize = m_reserveBackground.getTexture()->getSize();
    m_scaleFactor = size().y / textureSize.y;

    auto textureRectWidth  = static_cast<int>(m_reserveBackground.getSize().x / m_scaleFactor);
    auto textureRectHeight = static_cast<int>(textureSize.y);
    m_reserveBackground.setTextureRect({0, 0, textureRectWidth, textureRectHeight});

    // Puppets
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

//----------------------------//
//----- MonsterGrabbable -----//

MonsterGrabbable::MonsterGrabbable(scene::GrabbableSpawner& spawner, const std::wstring& monsterID, const sf::Vector2f& scale)
    : baseClass(spawner)
{
    attachChild(m_sprite);
    m_sprite.load(toString(L"dungeon/monsters/" + monsterID));
    m_sprite.setLocalScale(scale);
}
