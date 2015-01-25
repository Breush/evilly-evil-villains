#pragma once

// Forward declaration of SFML classes
namespace sf
{
    class Texture;
    class Shader;
    class Font;
    class SoundBuffer;
}

namespace Textures
{
    enum ID {
        NUI_FOCUS,
        MENU_BG,
        MENU_NAME,
    };
}

/*namespace Animations
{
    enum ID {
        CHARACTER_HERO,
    };
}*/

namespace Shaders
{
    enum ID {
        NONE,
        MENU_BG,
        MENU_NAME,
        NUI_HOVER,
        NUI_FOCUS,
    };
}

namespace Fonts
{
    enum ID {
        NUI,
    };
}

namespace Sounds
{
    enum ID {
        NUI_ACCEPT,
        NUI_REFUSE,
        NUI_SELECT,
    };
}

namespace Music
{
    enum ID {
        MENU_THEME,
    };
}

// Forward declaration and a few type definitions
template <typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Texture, Textures::ID>		TextureHolder;
typedef ResourceHolder<sf::Shader, Shaders::ID>			ShaderHolder;
typedef ResourceHolder<sf::Font, Fonts::ID>			    FontHolder;
typedef ResourceHolder<sf::SoundBuffer, Sounds::ID>	    SoundBufferHolder;
