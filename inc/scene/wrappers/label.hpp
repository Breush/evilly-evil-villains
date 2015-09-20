#pragma once

#include "scene/wrappers/tlabel.hpp"

#include <SFML/Graphics/Text.hpp>

namespace scene
{
    //! Label from sf::Text.
    using Label = TLabel<sf::Text>;
}
