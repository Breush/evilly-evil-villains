#include "hub/traplocker.hpp"

#include "dungeon/databases/trapsdb.hpp"
#include "tools/string.hpp"

using namespace hub;

TrapLocker::TrapLocker()
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
    m_lockedText.setPrestyle(scene::RichLabel::Prestyle::MENU_TITLE);

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

//-------------------//
//----- Control -----//

void TrapLocker::setSource(const std::wstring& trapID, const dungeon::TrapData& trapData)
{
    m_sprite.load("dungeon/traps/" + toString(trapID));
    m_sprite.select(L"presentation");
    m_name.setText(trapData.name);

    refreshFromLocking();
}

void TrapLocker::setLocked(bool locked)
{
    m_locked = locked;
    refreshFromLocking();
}

//-------------------//
//----- Control -----//

void TrapLocker::refreshFromLocking()
{
    m_sprite.setStarted(!m_locked);
    m_lockedText.setVisible(!m_locked);
    m_rectangle.setFillColor((m_locked)? sf::Color{0u, 0u, 0u, 200u} : sf::Color::Transparent);
}
