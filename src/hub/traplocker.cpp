#include "hub/traplocker.hpp"

#include "core/gettext.hpp"
#include "context/villains.hpp"
#include "dungeon/data.hpp"
#include "tools/string.hpp"

using namespace hub;

TrapLocker::TrapLocker(dungeon::Data& data)
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

void TrapLocker::onSizeChanges()
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

bool TrapLocker::handleMouseButtonPressed(const sf::Mouse::Button, const sf::Vector2f&, const sf::Vector2f&)
{
    returnif (!m_locked) true;
    returnif (!m_data.villain().doshWallet.sub(m_trapGeneric->common->unlockCost.dosh)) true;

    // TODO Yeah... well, have something more rewarding I guess
    Application::context().sounds.play("resources/dosh_gain");

    // We know the player has enough money and already paid, unlock it
    m_data.setTrapGenericUnlocked(m_trapID, true);
    setLocked(false);

    return true;
}

bool TrapLocker::handleMouseMoved(const sf::Vector2f&, const sf::Vector2f&)
{
    returnif (!m_locked) true;
    m_lockedIcon.setShader("nui/hover");
    return true;
}

void TrapLocker::handleMouseLeft()
{
    returnif (!m_locked);
    m_lockedIcon.setShader("");
}

//-------------------//
//----- Control -----//

void TrapLocker::setSource(const std::wstring& trapID, const dungeon::TrapGeneric& trapGeneric)
{
    m_trapID = trapID;
    m_trapGeneric = &trapGeneric;

    m_sprite.load("dungeon/traps/" + toString(trapID));
    m_sprite.select("presentation");
    m_name.setText(trapGeneric.common->name);
    m_lockedCostText.setText(L"*" + toWString(trapGeneric.common->unlockCost.dosh) + L"*");

    setLocked(!trapGeneric.unlocked);
}

void TrapLocker::setLocked(bool locked)
{
    m_locked = locked;
    refreshFromLocking();
}

//---------------//
//----- ICU -----//

void TrapLocker::refreshFromLocking()
{
    m_sprite.setStarted(!m_locked);
    m_lockedText.setVisible(m_locked);
    m_lockedIcon.setVisible(m_locked);
    m_rectangle.setFillColor(m_locked? sf::Color{0u, 0u, 0u, 220u} : sf::Color::Transparent);
}
