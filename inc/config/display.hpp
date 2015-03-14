#pragma once

#include <SFML/System/Vector2.hpp>

namespace config
{
    struct Display
    {
        //! Will load the corresponding config file.
        Display();

        //! Will save the data to the corresponding config file.
        ~Display();

        //! Whether the fullscreen is enabled.
        bool fullscreen;

        //! The resolution used to play the game.
        sf::Vector2f resolution;
    };
}
