#include "hub/monsterlocker.hpp"

#include "core/gettext.hpp"
#include "context/villains.hpp"
#include "dungeon/data.hpp"
#include "tools/string.hpp"

using namespace hub;

MonsterLocker::MonsterLocker(dungeon::Data& data)
    : m_data(data)
{
    // Name
    attachChild(m_name);
    m_name.setPrestyle(scene::RichLabel::Prestyle::NUI_TITLE);
    m_name.setRelativePosition({0.5f, 0.f});
    m_name.setRelativeOrigin({0.5f, 0.f});

    // Rectangle
    attachChild(m_rectangle);
    m_rectangle.setDepth(-50.f);
    m_rectangle.setOutlineColor(sf::Color::White);
    m_rectangle.setOutlineThickness(1.f);

    // Requirement
    m_rectangle.attachChild(m_requireCostBanner);
    m_requireCostBanner.setRelativePosition({0.1f, 0.5f});
    m_requireCostBanner.setRelativeOrigin({0.f, 0.5f});

    // Lock
    m_rectangle.attachChild(m_lockedText);
    m_lockedText.setPrestyle(scene::RichLabel::Prestyle::NUI);
    m_lockedText.setRelativePosition({0.5f, 0.5f});
    m_lockedText.setText(_("Locked"));
    m_lockedText.centerOrigin();

    m_lockedText.attachChild(m_lockedIcon);
    m_lockedIcon.setRelativePosition({-0.1f, 0.5f});
    m_lockedIcon.setRelativeOrigin({1.f, 0.5f});
    m_lockedIcon.setTexture("hub/general/lock_closed");
    m_lockedIcon.setSize({50.f, 50.f});

    m_lockedText.attachChild(m_lockedCostText);
    m_lockedCostText.setPrestyle(scene::RichLabel::Prestyle::NUI_TITLE);
    m_lockedCostText.setRelativePosition({1.2f, 0.5f});
    m_lockedCostText.setRelativeOrigin({0.f, 0.5f});

    m_lockedCostText.attachChild(m_lockedDoshIcon);
    m_lockedDoshIcon.setRelativePosition({1.f, 0.5f});
    m_lockedDoshIcon.setRelativeOrigin({0.f, 0.5f});
    m_lockedDoshIcon.setTexture("resources/dosh");
    m_lockedDoshIcon.setSize({32.f, 32.f});

    // Image
    attachChild(m_sprite);

    // The room
    addPart(&m_room);
    const auto& texture = Application::context().textures.get("hub/market/trap_room");
    m_roomTextureSize = sf::v2f(texture.getSize());
    m_room.setSize(m_roomTextureSize);
    m_room.setTexture(&texture);

    // TODO Add description

    refreshFromLocking();
}

//-------------------//
//----- Routine -----//

void MonsterLocker::onSizeChanges()
{
    // Border
    m_rectangle.setSize(size() - 2.f);

    // Sprite
    auto scale = size().y / m_roomTextureSize.y;
    m_sprite.setLocalPosition(0.5f * m_roomTextureSize * scale);
    m_sprite.setScale({scale, scale});
    m_room.setScale({scale, scale});
}

//------------------//
//----- Events -----//

bool MonsterLocker::handleMouseButtonPressed(const sf::Mouse::Button, const sf::Vector2f&, const sf::Vector2f&)
{
    returnif (!m_locked) true;

    // Unlock requirement
    returnif (m_data.doshWallet().value() < m_monsterGeneric->common->unlockRequirement.dosh) true;
    returnif (m_data.soulWallet().value() < m_monsterGeneric->common->unlockRequirement.soul) true;
    returnif (m_data.fameWallet().value() < m_monsterGeneric->common->unlockRequirement.fame) true;

    // Unlock cost
    // TODO Full cost (soul/fame)
    returnif (!m_data.doshWallet().sub(m_monsterGeneric->common->unlockCost.dosh)) true;

    // TODO See trap locker
    Application::context().sounds.play("resources/dosh_gain");

    // We know the player has enough money and already paid, unlock it
    m_data.setMonsterGenericUnlocked(m_monsterID, true);
    setLocked(false);

    return true;
}

bool MonsterLocker::handleMouseMoved(const sf::Vector2f&, const sf::Vector2f&)
{
    returnif (!m_locked) true;
    m_lockedIcon.setShader("nui/hover");
    return true;
}

void MonsterLocker::handleMouseLeft()
{
    returnif (!m_locked);
    m_lockedIcon.setShader("");
}

//-------------------//
//----- Control -----//

void MonsterLocker::setSource(const std::wstring& monsterID, const dungeon::MonsterGeneric& monsterGeneric)
{
    m_monsterID = monsterID;
    m_monsterGeneric = &monsterGeneric;

    m_sprite.load("dungeon/monsters/" + toString(monsterID));
    m_sprite.select(L"presentation");
    m_name.setText(monsterGeneric.common->name);
    m_lockedCostText.setText(L"*" + toWString(monsterGeneric.common->unlockCost.dosh) + L"*");

    m_requireCostBanner.setCost(monsterGeneric.common->unlockRequirement);

    setLocked(!monsterGeneric.unlocked);
}

void MonsterLocker::setLocked(bool locked)
{
    m_locked = locked;
    refreshFromLocking();
}

//---------------//
//----- ICU -----//

void MonsterLocker::refreshFromLocking()
{
    m_sprite.setStarted(!m_locked);
    m_lockedText.setVisible(m_locked);
    m_lockedIcon.setVisible(m_locked);
    m_requireCostBanner.setVisible(m_locked);
    m_rectangle.setFillColor((m_locked)? sf::Color{0u, 0u, 0u, 220u} : sf::Color::Transparent);
}
