#include "core/application.hpp"

void Application::loadMusics()
{
    // TODO Move to refreshSomething
    s_context.musics.setVolume(s_context.sound.effectiveMusicVolume);

    // Musics are too heavy too be preloaded
}
