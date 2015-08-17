#include "config/windowinfo.hpp"

using namespace config;

WindowInfo::WindowInfo(const Display::Window& cDisplayWindow)
    : resolution(cDisplayWindow.resolution)
{
}

void WindowInfo::recompute()
{
    viewport = {0.f, 0.f, 1.f, 1.f};

    // Quick check
    if (screenSize == resolution) {
        effectiveDisplay = resolution;
        return;
    };

    // Compute viewport
    // - Same ratio as resolution
    // - Centered into screenSize rect
    const auto viewRatio = screenSize / resolution;

    if (viewRatio.x > viewRatio.y) {
        viewport.width = viewRatio.y / viewRatio.x;
        viewport.left = (1.f - viewport.width) / 2.f;
    }
    else if (viewRatio.x < viewRatio.y) {
        viewport.height = viewRatio.x / viewRatio.y;
        viewport.top = (1.f - viewport.height) / 2.f;
    }

    // Effective display is real dimension in pixels of the drawable area.
    effectiveDisplay = {screenSize.x * viewport.width, screenSize.y * viewport.height};
}
