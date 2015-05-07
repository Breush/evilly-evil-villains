#include "dungeon/traps/pickpock.hpp"

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

    // Ensures that event occured in the same room than us.
    sf::Vector2u coords(event.action.room.x, event.action.room.y);
    returnif (coords != m_room->coords);

    std::cerr << "[Pick-pock] Steals money from " << event.action.hero << " in room " << coords << std::endl;
}
