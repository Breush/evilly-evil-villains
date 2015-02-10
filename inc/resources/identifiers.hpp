#pragma once

//----- Forward declarations

namespace sf
{
    class Texture;
    class Shader;
    class Font;
    class SoundBuffer;
}

namespace scml
{
    class Data;
}

//----- Effective defines

namespace Textures
{
    enum ID {
        DEFAULT,
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
        DUNGEON_SCENE_GRASSYHILLS_BACK,
        DUNGEON_SCENE_GRASSYHILLS_FRONT,
        DUNGEON_PANEL_BACKGROUND,
        DUNGEON_PANEL_SWITCH,
        DUNGEON_PANEL_MONSTERS,
        DUNGEON_PANEL_TRAPS,
        DUNGEON_PANEL_FACILITIES,
        DUNGEON_PANEL_TREASURES,
    };
}

namespace Animations
{
    enum ID {
        DEFAULT,
        JUMPINGTOASTS,
    };
}

namespace Shaders
{
    enum ID {
        DEFAULT,
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
        DEFAULT,
        NUI,
    };
}

namespace Sounds
{
    enum ID {
        DEFAULT,
        JUMPINGTOASTS,
        NUI_ACCEPT,
        NUI_REFUSE,
        NUI_SELECT,
    };
}

namespace Musics
{
    enum ID {
        DEFAULT,
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
typedef ResourceHolder<scml::Data, Animations::ID>	    SCMLHolder;
