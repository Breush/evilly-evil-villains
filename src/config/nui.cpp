#include "config/nui.hpp"

#include "config/display.hpp"

using namespace config;

NUI::NUI()
{
    config::Display displayConfig;
    auto nuiSize = displayConfig.nui;

    hPadding = 3.f * nuiSize;
    vPadding = 3.f * nuiSize;

    borderThick = 1.f;
}
