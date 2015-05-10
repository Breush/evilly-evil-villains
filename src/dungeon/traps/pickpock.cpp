#include "dungeon/traps/pickpock.hpp"

#include "dungeon/hero.hpp"
#include "tools/tools.hpp"
#include "tools/debug.hpp"
#include "tools/vector.hpp"

using namespace dungeon::traps;

PickPock::PickPock(const Data::Room& room)
    : baseClass(room)
{
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
    returnif (coords != m_room->coords);

    // If so, steal the hero
    stealFromHero(event.action.hero);
}

//--------------------//
//----- Stealing -----//

void PickPock::stealFromHero(Hero* hero)
{
    auto doshStolen = std::min(m_maxDosh - dosh(), hero->dosh());
    hero->subDosh(doshStolen);
    addDosh(doshStolen);

    Event event;
    event.type = EventType::HARVESTABLE_DOSH_CHANGED;
    event.room = {m_room->coords.x, m_room->coords.y};
    emitter()->emit(event);
}
