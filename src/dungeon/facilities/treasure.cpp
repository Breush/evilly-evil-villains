#include "dungeon/facilities/treasure.hpp"

#include "resources/identifiers.hpp"
#include "tools/tools.hpp"

using namespace dungeon::facilities;

Treasure::Treasure(const sf::Vector2u& coords, ElementData& elementdata)
    : baseClass(coords, elementdata)
{
    // Initialize attributes if they do not exists yet,
    // i.e. we just constructed this trap, and not loaded it from XML.
    if (m_elementdata.empty()) {
        m_elementdata[L"dosh"].init_uint32(0u);
    }

    // Decorum
    attachChild(m_sprite);
    m_sprite.load(AnimationID::DUNGEON_FACILITIES_TREASURE);
}
