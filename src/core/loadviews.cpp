#include "core/application.hpp"

#include "resources/identifiers.hpp"

// We set view to keep the same ratio than resolution, keeping previous zoom and center
void adaptViewToResolution(sf::View& view, const sf::Vector2f& resolution)
{
    const auto& viewSize = view.getSize();
    float zoomFactor = (viewSize.x > viewSize.y)? viewSize.y / resolution.y : viewSize.x / resolution.x;
    view.setSize(resolution * zoomFactor);
}

void Application::loadViews()
{
    // TODO Currently, the views are stored using the ResourceHolder.
    // This can be pricy because of the map searching and the pointer indirection.
    // Should be more clever to use std::array<sf::View> directly.

    s_context.views.load(ViewID::DEFAULT);
    s_context.views.load(ViewID::DUNGEON_DESIGN);
    s_context.views.load(ViewID::NUI);

    refreshViews();
}

void Application::refreshViews()
{
    const auto& screenSize = s_context.screenSize;
    const auto& resolution = s_context.resolution;

    sf::FloatRect viewport(0.f, 0.f, 1.f, 1.f);
    const sf::Vector2f viewRatio = sf::vdiv(screenSize, resolution);

    if (viewRatio.x > viewRatio.y) {
        viewport.width = viewRatio.y / viewRatio.x;
        viewport.left = (1.f - viewport.width) / 2.f;
    }
    else if (viewRatio.x < viewRatio.y) {
        viewport.height = viewRatio.x / viewRatio.y;
        viewport.top = (1.f - viewport.height) / 2.f;
    }

    s_context.effectiveDisplay = sf::Vector2f(screenSize.x * viewport.width, screenSize.y * viewport.height);

    // Refresh all views
    auto& defaultView = s_context.views.get(ViewID::DEFAULT);
    defaultView.setCenter(resolution / 2.f);
    defaultView.setSize(resolution);
    defaultView.setViewport(viewport);

    auto& dungeonDesignView = s_context.views.get(ViewID::DUNGEON_DESIGN);
    adaptViewToResolution(dungeonDesignView, resolution);
    dungeonDesignView.setViewport(viewport);

    auto& nuiView = s_context.views.get(ViewID::NUI);
    nuiView = defaultView;
}
