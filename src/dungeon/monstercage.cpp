#include "dungeon/monstercage.hpp"

#include "core/application.hpp"
#include "context/villains.hpp"
#include "dungeon/inter.hpp"
#include "dungeon/data.hpp"
#include "tools/vector.hpp"

using namespace dungeon;

MonsterCage::MonsterCage(std::wstring monsterID, Data& data)
    : m_data(data)
    , m_monsterID(std::move(monsterID))
{
    // Events
    setEmitter(&data);

    // Background
    addPart(&m_background);
    m_background.setTexture(&Application::context().textures.get("dungeon/sidebar/tab/monsters/cage"));

    // Reserve
    // TODO Have X puppets because there is X monsters in the reserve
    attachChild(m_monsterPuppet);
    m_monsterPuppet.setSource(toString(L"dungeon/monsters/" + m_monsterID));
    m_monsterPuppet.lerpable()->setPositionSpeed({100.f, 100.f});

    // Cost box
    attachChild(m_baseCostLabel);
    const auto& monsterData = m_data.monstersDB().get(m_monsterID);
    m_baseCostLabel.setPrestyle(scene::RichLabel::Prestyle::NUI);
    m_baseCostLabel.setText(toWString(monsterData.baseCost.dosh));
    refreshCostLabelsColor();
}

//-------------------//
//----- Routine -----//

void MonsterCage::onSizeChanges()
{
    // Background, fit texture to height
    m_background.setSize(size());
    const auto& textureSize = m_background.getTexture()->getSize();
    float scaleFactor = size().y / textureSize.y;

    auto textureRectWidth  = static_cast<int>(size().x / scaleFactor);
    auto textureRectHeight = static_cast<int>(textureSize.y);
    m_background.setTextureRect({0, 0, textureRectWidth, textureRectHeight});

    // Puppets
    // TODO Why 25.f? Use collision box, position 0.f means 0.f + colBox
    m_monsterPuppet.setInitialLocalPosition({25.f, 0.62f * size().y});
    m_monsterPuppet.setHorizontalRange(25.f, size().x - 25.f);
    m_monsterPuppet.setLocalScale({scaleFactor, scaleFactor});
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

void MonsterCage::grabbableButtonReleased(Entity* entity, const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos)
{
    returnif (button != sf::Mouse::Left);
    graph()->removeGrabbable();

    // Forward to dungeon::Inter if it is below
    auto inter = dynamic_cast<Inter*>(entity);
    returnif (inter == nullptr);
    inter->addMonster(relPos, m_monsterID);
}

std::unique_ptr<scene::Grabbable> MonsterCage::spawnGrabbable()
{
    return std::make_unique<MonsterGrabbable>(*this, m_monsterID);
}

//-----------------------------------//
//----- Internal change updates -----//

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
    // FIXME This a trick because scene::AnimatedSprite does not consider states.transform
    const_cast<scene::AnimatedSprite&>(m_sprite).setLocalPosition(getPosition());

    //states.transform *= getTransform();
    target.draw(m_sprite, states);
}