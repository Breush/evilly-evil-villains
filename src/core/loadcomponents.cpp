#include "core/application.hpp"

#include "context/componenter.hpp"
#include "scene/components/ai.hpp"
#include "scene/components/lerpable.hpp"
#include "scene/components/lightemitter.hpp"

void Application::loadComponents()
{
    context::componenter.registerComponentType<scene::AI>();
    context::componenter.registerComponentType<scene::Lerpable>();
    context::componenter.registerComponentType<scene::LightEmitter>();
}

// FIXME No more game factor.
// It should probably be stored in each entity or accessible via entity->layer->factor.
void Application::updateComponents(const sf::Time& dt)
{
    // context::componenter.update<scene::AI>(dt);
    context::componenter.update<scene::Lerpable>(dt);
    // context::componenter.update<scene::LightEmitter>(dt);
}

void Application::freeComponents()
{
    context::componenter.unregisterComponentType<scene::AI>();
    context::componenter.unregisterComponentType<scene::Lerpable>();
    context::componenter.unregisterComponentType<scene::LightEmitter>();
}
