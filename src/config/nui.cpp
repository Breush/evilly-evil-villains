#include "config/nui.hpp"

#include "core/application.hpp"

using namespace config;

NUI::NUI()
{
    auto nuiSize = Application::context().display.nui;

    hPadding = 1.f + 3.f * nuiSize;
    vPadding = 1.f + 3.f * nuiSize;

    borderThick = 1.f;

    fontSize = 10.f + nuiSize * 3.f;
    fontVSpace = 1.2f * fontSize;

    hintSize = 9.f + nuiSize * 2.f;
}

