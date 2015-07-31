#include "core/application.hpp"

#include "resources/identifiers.hpp"

void Application::loadMusics()
{
    s_context.musics.setVolume(s_context.sound.effectiveMusicVolume);

    s_context.musics.load(MusicID::MENU_THEME, "res/mus/menu_theme.ogg");
}
