#include "dungeon/traps/pickpock.hpp"

#include "resources/identifiers.hpp"
#include "dungeon/hero.hpp"
#include "dungeon/data.hpp"
#include "tools/tools.hpp"
#include "tools/debug.hpp"
#include "tools/vector.hpp"

using namespace dungeon::traps;

PickPock::PickPock(const sf::Vector2u& coords, ElementData& elementdata)
    : baseClass(coords, elementdata)
{
    // Initialize attributes if they do not exists yet,
    // i.e. we just constructed this trap, and not loaded it from XML.
    if (m_elementdata.empty()) {
        m_elementdata[L"dosh"].init_uint32(0u);
    }

    // Decorum
    attachChild(m_sprite);
    m_sprite.load(AnimationID::DUNGEON_TRAPS_PICKPOCK);
}

//-------------------------//
//---- Dungeon events -----//

void PickPock::receive(const Event& event)
{
    returnif (event.type != EventType::HERO_ENTERED_ROOM);

    // Ensures that event occured in the same room than us
    sf::Vector2u coords(event.action.room.x, event.action.room.y);
    if (coords != m_coords) {
        m_sprite.select(L"idle");
        m_sprite.setLooping(true);
    }
    else {
        stealFromHero(event.action.hero);
    }

    // TODO That select "idle" is a bit heavy,
    // just have a check that the hero has left.
    // + that way, we will not steal 2 heroes at once.
}

//--------------------//
//----- Resources -----//

void PickPock::setDosh(uint32 value)
{
    m_elementdata[L"dosh"].as_uint32() = value;

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

    if (doshStolen != 0u) {
        m_sprite.select(L"grab");
        m_sprite.setLooping(false);
        // TODO Animate hero somehow
        hero->subDosh(doshStolen);
        addDosh(doshStolen);
    }
}

