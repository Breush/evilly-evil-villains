#include "dungeon/traps/pickpock.hpp"

#include "dungeon/hero.hpp"
#include "dungeon/data.hpp"
#include "tools/tools.hpp"
#include "tools/debug.hpp"
#include "tools/vector.hpp"

using namespace dungeon::traps;

PickPock::PickPock(const sf::Vector2u& coords, TrapData& trapdata)
    : baseClass(coords, trapdata)
{
    // Initialize attributes if they do not exists yet,
    // i.e. we just constructed this trap, and not loaded it from XML.
    if (m_trapdata.empty()) {
        m_trapdata[L"dosh"].init_uint32(0u);
    }

    // Decorum
    // TODO Have a animated sprite
    m_sprite.setFillColor(sf::Color::Green);
    addPart(&m_sprite);

    setSize({25.f, 25.f});
}

//------------------//
//---- Routine -----//

void PickPock::update()
{
    m_sprite.setSize(size());
}

//-------------------------//
//---- Dungeon events -----//

void PickPock::receive(const Event& event)
{
    returnif (event.type != EventType::HERO_ENTERED_ROOM);

    // Ensures that event occured in the same room than us
    sf::Vector2u coords(event.action.room.x, event.action.room.y);
    returnif (coords != m_coords);

    // If so, steal the hero
    stealFromHero(event.action.hero);
}

//--------------------//
//----- Resources -----//

void PickPock::setDosh(uint32 value)
{
    m_trapdata[L"dosh"].as_uint32() = value;

    Event event;
    event.type = EventType::HARVESTABLE_DOSH_CHANGED;
    event.room = {m_coords.x, m_coords.y};
    emitter()->emit(event);
}

//--------------------//
//----- Stealing -----//

void PickPock::stealFromHero(Hero* hero)
{
    auto doshStolen = std::min(m_maxDosh - dosh(), hero->dosh());
    hero->subDosh(doshStolen);
    addDosh(doshStolen);
}

