#include "dungeon/facilities/treasure.hpp"

#include "dungeon/inter.hpp"
#include "context/villains.hpp"
#include "tools/tools.hpp"

using namespace dungeon::facilities;

Treasure::Treasure(const sf::Vector2u& coords, ElementData& elementdata, dungeon::Inter& inter)
    : baseClass(coords, elementdata, inter)
{
    setDepth(80.f);

    // Initialize attributes if they do not exists yet,
    // i.e. we just constructed this, and not loaded it from XML.
    if (m_elementdata.empty()) {
        m_elementdata[L"dosh"].init_uint32(0u);
        if (m_inter.villain().doshWallet.sub(100u))
            m_elementdata[L"dosh"].as_uint32() = 100u;
    }

    // Decorum
    attachChild(m_sprite);
    m_sprite.load("dungeon/facilities/treasure");
}
