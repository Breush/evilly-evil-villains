#pragma once

#include "config/display.hpp"
#include "tools/vector.hpp"
#include "tools/int.hpp"

namespace config
{
    //! Extra information for window.

    struct WindowInfo
    {
        //! Constructor. The display file is binded.
        WindowInfo(const Display::Window& cDisplayWindow);

        //! Initialize all the parameters, according to current externally managed elements.
        void recompute();

        // Externally managed
        const sf::Vector2f& resolution; //!< Resolution binded from config file.
        sf::Vector2f screenSize;        //!< The dimension of the drawable part of the window.
        std::string title;              //!< The title of the window.
        uint32 style;                   //!< The style of the window, as defined by SFML.

        // Computed
        sf::FloatRect viewport;         //!< The current viewport.
        sf::Vector2f effectiveDisplay;  //!< The dimensions of the viewport centered for the window, same ratio as resolution.
    };
}
