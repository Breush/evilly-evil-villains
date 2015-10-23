#include "core/application.hpp"

void Application::loadMusics()
{
    // Musics are too heavy too be preloaded
    // so there streamed live on call.

    refreshMusics();
}

void Application::refreshMusics()
{
    s_context.musics.setVolume(s_context.sound.effectiveMusicVolume);
}
