#include "core/application.hpp"

#include "resources/identifiers.hpp"

void Application::loadAnimations()
{
    // Splash-screen
    s_context.animations.load(AnimationID::JUMPINGTOASTS, "res/scml/jumping-toasts.scml");

    // Dungeon traps
    s_context.animations.load(AnimationID::DUNGEON_TRAPS_PICKPOCK, "res/scml/dungeon/traps/pickpock.scml");

    // Dungeon facilities
    s_context.animations.load(AnimationID::DUNGEON_FACILITIES_ENTRANCE, "res/scml/dungeon/facilities/entrance.scml");
    s_context.animations.load(AnimationID::DUNGEON_FACILITIES_TREASURE, "res/scml/dungeon/facilities/treasure.scml");

    // Heroes
    s_context.animations.load(AnimationID::HEROES_GROO, "res/scml/heroes/groo.scml");
}

void Application::updateAnimations(const sf::Time& dt)
{
    s_context.animations.update(dt);
}
