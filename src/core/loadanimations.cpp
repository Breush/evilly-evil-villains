#include "core/application.hpp"

#include "resources/identifiers.hpp"

void Application::loadAnimations()
{
    // Splash-screen
    s_context.animations.load(AnimationID::JUMPINGTOASTS, "res/scml/jumping-toasts.scml");

    // Heroes
    s_context.animations.load(AnimationID::HEROES_GROO, "res/scml/heroes/groo.scml");
}

void Application::updateAnimations(const sf::Time& dt)
{
    s_context.animations.update(dt);
}
