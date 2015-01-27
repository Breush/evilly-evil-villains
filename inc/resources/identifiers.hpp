#pragma once

//----- Forward declarations

namespace sf
{
    class Texture;
    class Shader;
    class Font;
    class SoundBuffer;
}

namespace SCML
{
    class Data;
}

//----- Effective defines

namespace Textures
{
    enum ID {
        JUMPINGTOASTS_BG,
        JUMPINGTOASTS_JUMPING,
        JUMPINGTOASTS_TOASTS,
        JUMPINGTOASTS_TOAST,
        JUMPINGTOASTS_TOASTCUT,
        JUMPINGTOASTS_TOASTERBACKGROUND,
        JUMPINGTOASTS_TOASTERBOTTOM,
        JUMPINGTOASTS_TOASTERMIDDLE,
        JUMPINGTOASTS_TOASTERTOP,
        NUI_FOCUS,
        MENU_BG,
        MENU_NAME,
    };
}

namespace Animations
{
    enum ID {
        JUMPINGTOASTS,
    };
}

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
        JUMPINGTOASTS,
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

//----- Few type definitions

template <typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Texture, Textures::ID>		TextureHolder;
typedef ResourceHolder<sf::Shader, Shaders::ID>			ShaderHolder;
typedef ResourceHolder<sf::Font, Fonts::ID>			    FontHolder;
typedef ResourceHolder<sf::SoundBuffer, Sounds::ID>	    SoundBufferHolder;
typedef ResourceHolder<SCML::Data, Animations::ID>	    SCMLHolder;
