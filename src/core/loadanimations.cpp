#include "core/application.hpp"

void Application::loadAnimations()
{
    // Splash-screen
    s_context.animations.load(Animations::JUMPINGTOASTS, "res/scml/jumping-toasts.scml");
}

void Application::updateAnimations(const sf::Time& dt)
{
    s_context.animations.update(dt);
}
