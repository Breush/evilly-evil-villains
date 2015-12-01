#include "dungeon/sidebar/monstercage.hpp"

#include "core/gettext.hpp"
#include "core/application.hpp"
#include "context/villains.hpp"
#include "dungeon/inter.hpp"
#include "dungeon/data.hpp"
#include "tools/vector.hpp"
#include "tools/random.hpp"

#include <Box2D/Box2D.hpp>

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
    m_nameLabel.setRelativePosition({0.8f, 0.f});
    m_nameLabel.setRelativeOrigin({0.5f, 0.f});

    // Hire box
    attachChild(m_hireBoxBackground);
    m_hireBoxBackground.setDepth(100.f);
    m_hireBoxBackground.setOutlineThickness(1.f);
    m_hireBoxBackground.setOutlineColor(sf::Color::White);
    m_hireBoxBackground.setFillColor({0u, 0u, 0u, 185u});
    m_hireBoxBackground.setRelativeOrigin({1.f, 1.f});

    m_hireBoxBackground.attachChild(m_hireBoxLabel);
    m_hireBoxLabel.setFont("mono");
    m_hireBoxLabel.setCustomPrestyle(0.8f);
    m_hireBoxLabel.setRelativeOrigin({0.5f, 1.f});
    m_hireBoxLabel.setRelativePosition({0.5f, 0.95f});

    // Cost banners
    m_hireBoxBackground.attachChild(m_baseCostBanner);
    m_baseCostBanner.setTitle(_("Hire"));
    m_baseCostBanner.setCost(monsterData.baseCost);
    m_baseCostBanner.setRelativePosition({0.1f, 0.25f});

    m_hireBoxBackground.attachChild(m_weeklyCostBanner);
    m_weeklyCostBanner.setTitle(_("Weekly"));
    m_weeklyCostBanner.setCost(monsterData.weeklyCost);
    m_weeklyCostBanner.setRelativePosition({0.99f, 0.25f});
    m_weeklyCostBanner.setRelativeOrigin({1.f, 0.f});

    refreshReservePuppetsCount();
}

//-------------------//
//----- Routine -----//

void MonsterCage::onSizeChanges()
{
    // Hire box background, taking outline in account
    m_hireBoxBackground.setLocalPosition(size() - 1.f);
    m_hireBoxBackground.setSize({0.4f * size().x - 2.f, size().y - 2.f});

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

    else if (devent.type == "reserve_countdown_changed" && devent.monster.id == m_monsterID)
        refreshHireBox();
}

bool MonsterCage::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    returnif (button != sf::Mouse::Left) false;

    // Cage part
    if (mousePos.x < 0.6f * size().x)
        graph()->setGrabbable(spawnGrabbable());

    // Hire part
    else
        hire();

    return true;
}

bool MonsterCage::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    m_hireBoxSelected = (mousePos.x > 0.6f * size().x);
    refreshHireBoxVisualLock();
    return true;
}

void MonsterCage::handleMouseLeft()
{
    m_hireBoxSelected = false;
    refreshHireBoxVisualLock();
}

//---------------------//
//----- Grabbable -----//

void MonsterCage::grabbableMoved(Entity* entity, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos)
{
    auto offset = m_grabbablePosition - nuiPos;
    reinterpret_cast<MonsterGrabbable*>(graph()->grabbable())->movedOffset(offset);
    m_grabbablePosition = nuiPos;

    // Forward to dungeon::Inter if it is below
    sf::Vector2f interRelPos(relPos);
    returnif (entity == nullptr);
    auto inter = entity->findParent<Inter>(interRelPos);

    if (inter == nullptr) {
        m_inter.resetPrediction();
        return;
    }

    m_inter.setPredictionMonster(interRelPos, m_monsterID);
}

void MonsterCage::grabbableButtonReleased(Entity* entity, const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left);
    m_puppets.back()->setVisible(true);
    graph()->removeGrabbable();
    m_inter.resetPrediction();

    // Forward to dungeon::Inter if it is below
    sf::Vector2f interRelPos(relPos);
    returnif (entity == nullptr);
    auto inter = entity->findParent<Inter>(interRelPos);
    returnif (inter == nullptr);

    m_inter.moveMonsterFromReserve(interRelPos, m_monsterID);
}

std::unique_ptr<scene::Grabbable> MonsterCage::spawnGrabbable()
{
    // Grab if puppets available
    returnif (m_puppets.size() == 0u) nullptr;

    m_grabbablePosition = sf::v2f(sf::Mouse::getPosition());

    // And hide one puppet, as it is being grabbed
    m_puppets.back()->setVisible(false);
    return std::make_unique<MonsterGrabbable>(*this, m_monsterID, 0.5f * m_inter.roomScale());
}

//------------------//
//----- Hiring -----//

void MonsterCage::hire()
{
    const auto& reserve = m_data.monstersInfo().reserve;
    auto pCage = std::find_if(reserve, [this] (const Data::MonsterCageInfo& monsterCage) { return monsterCage.type == m_monsterID; });
    returnif (pCage == std::end(reserve));

    // Check that the monster can be hired
    returnif (pCage->countdown != 0u);

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
    m_hireBoxLabel.setText(L"*Hire*  " + toWString(pCage->countdown));

    // Unlock
    m_hireBoxLocked = (pCage->countdown > 0u);
    refreshHireBoxVisualLock();
}

void MonsterCage::refreshHireBoxVisualLock()
{
    if (m_hireBoxLocked)
        m_hireBoxBackground.setFillColor({255u, 105u, 97u, 185u});
    else if (m_hireBoxSelected)
        m_hireBoxBackground.setFillColor({175u, 198u, 207u, 185u});
    else
        m_hireBoxBackground.setFillColor({0u, 0u, 0u, 185u});
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
    auto reserveSize = sf::Vector2f(0.6f * size().x, size().y);

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

// FIXME To be refatored inside MonsterGrabbable or in Grabbable itself
b2World s_world({0.f, -9.81f});
b2Body* s_staticBody;
b2Body* s_body;

bool init = false;

MonsterGrabbable::MonsterGrabbable(scene::GrabbableSpawner& spawner, const std::wstring& monsterID, const sf::Vector2f& scale)
    : baseClass(spawner)
{
    attachChild(m_sprite);
    m_sprite.load(toString(L"dungeon/monsters/" + monsterID));
    m_sprite.setLocalScale(scale);
    m_sprite.select(L"grabbed");

    if (!init) {
        init = true;

        b2BodyDef bodyDef;
        bodyDef.position = {0.f, 0.f};
        bodyDef.type = b2_dynamicBody;
        s_body = s_world.CreateBody(&bodyDef);

        b2PolygonShape shape;
        shape.SetAsBox(0.5f, 1.f, {0.5f, 0.f}, 0.f);

        b2FixtureDef fixtureDef;
        fixtureDef.density = 1.f;
        fixtureDef.shape = &shape;
        s_body->CreateFixture(&fixtureDef);

        b2CircleShape weightShape;
        weightShape.m_p = {0.f, 2.f};
        weightShape.m_radius = 0.25f;

        b2FixtureDef fixtureWeightDef;
        fixtureWeightDef.density = 300.f;
        fixtureWeightDef.shape = &weightShape;
        s_body->CreateFixture(&fixtureWeightDef);

        b2BodyDef staticBodyDef;
        staticBodyDef.position = {0.f, 0.f};
        staticBodyDef.type = b2_staticBody;
        s_staticBody = s_world.CreateBody(&staticBodyDef);

        b2RevoluteJointDef revoluteJointDef;
        revoluteJointDef.bodyA = s_body;
        revoluteJointDef.bodyB = s_staticBody;
        revoluteJointDef.localAnchorA = {0.f, 0.f};
        revoluteJointDef.localAnchorB = {0.f, 0.f};
        revoluteJointDef.enableMotor = true;
        revoluteJointDef.motorSpeed = 0.f;
        revoluteJointDef.maxMotorTorque = 8.f;

        s_world.CreateJoint(&revoluteJointDef);
    }

    s_body->SetTransform({0.f, 0.f}, 0.f);
    s_body->SetAngularVelocity(0.f);
}

void MonsterGrabbable::updateRoutine(const sf::Time& dt)
{
    s_world.Step(dt.asSeconds(), 8, 3);

    auto rotation = s_body->GetAngle() * 180.f / b2_pi;
    m_sprite.setLocalRotation(rotation);
}

void MonsterGrabbable::movedOffset(const sf::Vector2f& dir)
{
    float bodyAngle = s_body->GetAngle();
    float desiredAngle = std::atan2(dir.x, dir.y);
    float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);

    s_body->ApplyAngularImpulse((desiredAngle - bodyAngle) * length, true);
}
