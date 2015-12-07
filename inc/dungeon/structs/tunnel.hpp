#pragma once

#include <SFML/System/Vector2.hpp>

namespace dungeon
{
    //! A one-way access between two rooms.
    struct Tunnel
    {
        sf::Vector2i coords;    //!< The target room coords (negative coordinates are allowed if relative is on).
        bool relative = false;  //!< Are the target coords relative to the original room?
    };
}
