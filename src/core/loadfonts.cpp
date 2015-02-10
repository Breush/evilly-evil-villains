#include "core/application.hpp"

void Application::loadFonts()
{
    s_context.fonts.load(Fonts::NUI, "res/font/dream_orphans.ttf");
}

