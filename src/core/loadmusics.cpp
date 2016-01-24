#include "core/application.hpp"

#include "context/context.hpp"

void Application::loadMusics()
{
    // Musics are too heavy too be preloaded
    // so they're streamed live on call.

    refreshMusics();
}

void Application::refreshMusics()
{
    context::context.musics.setVolume(context::context.audio.effectiveMusicVolume);
}
