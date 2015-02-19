#include "core/application.hpp"

#include "resources/identifiers.hpp"

void Application::loadAnimations()
{
    // Splash-screen
    s_context.animations.load(AnimationID::JUMPINGTOASTS, "res/scml/jumping-toasts.scml");
}

void Application::updateAnimations(const sf::Time& dt)
{
    s_context.animations.update(dt);
}
