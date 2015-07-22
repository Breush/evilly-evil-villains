#pragma once

// Return if inline statement
#define returnif(...) if (__VA_ARGS__) return

#include <SFML/Graphics/RenderTarget.hpp>

#include <map>

namespace tools
{
    template<typename T1, typename T2>
    inline T2 mapFind(const std::map<T1, T2>& map, T1 const& key)
    {
        const auto& element = map.find(key);
        if (element == map.end()) return T2();
        return element->second;
    }

    // TODO Move to its own file?
    inline sf::FloatRect mapRectCoordsToPixel(const sf::RenderTarget& target, const sf::View& view, const sf::FloatRect& rect)
    {
        sf::FloatRect pixelRect;
        auto topLeft = target.mapCoordsToPixel({rect.left, rect.top}, view);
        auto bottomRight = target.mapCoordsToPixel({rect.left + rect.width, rect.top + rect.height}, view);
        pixelRect.left = topLeft.x;
        pixelRect.top = topLeft.y;
        pixelRect.width = bottomRight.x - topLeft.x;
        pixelRect.height = bottomRight.y - topLeft.y;
        return pixelRect;
    }
}
