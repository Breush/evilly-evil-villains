#include "config/nui.hpp"

#include "core/application.hpp"

using namespace config;

NUI::NUI()
{ 
    auto nuiSize = Application::context().display.nui;

    hPadding = 3.f * nuiSize;
    vPadding = 3.f * nuiSize;

    borderThick = 1.f;
}

