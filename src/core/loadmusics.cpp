#include "core/application.hpp"

void Application::loadMusics()
{
    s_context.musics.load(Musics::MENU_THEME, "res/mus/menu_theme.ogg");
}
