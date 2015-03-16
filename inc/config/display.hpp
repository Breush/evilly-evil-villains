#pragma once

#include "tools/int.hpp"

#include <SFML/System/Vector2.hpp>

namespace config
{
    struct Display
    {
        //! Load the corresponding config file.
        Display();

        //! Save the data to the corresponding config file.
        void save();

        bool fullscreen;            //!< Whether the fullscreen is enabled.
        sf::Vector2f resolution;    //!< The resolution used to play the game.
        uint nui;                   //!< The size of the user interface.
    };
}

