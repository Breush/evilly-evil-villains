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
    inline sf::FloatRect mapRectCoordsToPixel(const sf::RenderTarget& target, const sf::FloatRect& rect, const sf::View* view = nullptr)
    {
        // Get coords
        sf::Vector2i topLeft, bottomRight;
        if (view != nullptr) {
            topLeft = target.mapCoordsToPixel({rect.left, rect.top}, *view);
            bottomRight = target.mapCoordsToPixel({rect.left + rect.width, rect.top + rect.height}, *view);
        }
        else {
            topLeft = target.mapCoordsToPixel({rect.left, rect.top});
            bottomRight = target.mapCoordsToPixel({rect.left + rect.width, rect.top + rect.height});
        }

        // Compute rect
        sf::FloatRect pixelRect;
        pixelRect.left = topLeft.x;
        pixelRect.top = topLeft.y;
        pixelRect.width = bottomRight.x - topLeft.x;
        pixelRect.height = bottomRight.y - topLeft.y;
        return pixelRect;
    }
}
