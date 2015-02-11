#include "core/application.hpp"

void Application::loadViews()
{
    // TODO Currently, the views are stored using the ResourceHolder.
    // This can be pricy because of the map searching and the pointer indirection.
    // Should be more clever to use std::array<sf::View> directly.

    s_context.views.load(Views::DEFAULT);
    s_context.views.load(Views::DUNGEON_DESIGN);

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
    sf::View& defaultView(s_context.views.get(Views::DEFAULT));
    defaultView.setCenter(resolution / 2.f);
    defaultView.setSize(resolution);
    defaultView.setViewport(viewport);

    sf::View& dungeonDesignView(s_context.views.get(Views::DUNGEON_DESIGN));
    dungeonDesignView.setViewport(viewport);
}
