#include "config/nuiguides.hpp"

using namespace config;

void NUIGuides::recompute(const Display::NUI& cDisplayNUI)
{
    const auto& nuiSize = cDisplayNUI.size;
    const auto& fontFactor = cDisplayNUI.fontFactor;

    hPadding = 2.f + 4.f * nuiSize;
    vPadding = 3.f + 6.f * nuiSize;

    borderThick = 1.f;

    fontSize = 10.f + fontFactor * nuiSize;
    fontVSpace = 1.2f * fontSize;
    fontHSpace = 0.6f * fontSize;

    hintSize = 9.f + 2.f * nuiSize;
    hintImageSide = 30.f + 2.f * hintSize;
}