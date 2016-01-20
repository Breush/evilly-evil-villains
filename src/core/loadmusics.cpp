#include "core/application.hpp"

void Application::loadMusics()
{
    // Musics are too heavy too be preloaded
    // so they're streamed live on call.

    refreshMusics();
}

void Application::refreshMusics()
{
    s_context.musics.setVolume(s_context.audio.effectiveMusicVolume);
}
