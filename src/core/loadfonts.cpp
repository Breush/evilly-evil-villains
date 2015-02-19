#include "core/application.hpp"

#include "resources/identifiers.hpp"

void Application::loadFonts()
{
    s_context.fonts.load(FontID::NUI, "res/font/dream_orphans.ttf");
    s_context.fonts.load(FontID::MONO, "res/font/monofur.ttf");
}

