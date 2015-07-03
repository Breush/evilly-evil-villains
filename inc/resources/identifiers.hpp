#pragma once

#include "tools/int.hpp"

enum class TextureID : uint8
{
    DEFAULT,
    JUMPINGTOASTS_BACKGROUND,
    JUMPINGTOASTS_JUMPING,
    JUMPINGTOASTS_TOASTS,
    JUMPINGTOASTS_TOAST,
    JUMPINGTOASTS_TOASTCUT,
    JUMPINGTOASTS_TOASTERBACKGROUND,
    JUMPINGTOASTS_TOASTERBOTTOM,
    JUMPINGTOASTS_TOASTERMIDDLE,
    JUMPINGTOASTS_TOASTERTOP,
    NUI_FOCUS,
    MENU_MAIN_BACKGROUND,
    MENU_NAME,
    DUNGEON_SCENE_GRASSYHILLS_BACK,
    DUNGEON_SCENE_GRASSYHILLS_MIDDLE,
    DUNGEON_SCENE_GRASSYHILLS_FRONT,
    DUNGEON_PANEL_BACKGROUND,
    DUNGEON_PANEL_SWITCH,
    DUNGEON_PANEL_MONSTERS,
    DUNGEON_PANEL_TRAPS,
    DUNGEON_PANEL_FACILITIES,
    DUNGEON_MONSTERS_CREEPIM,
    DUNGEON_TRAPS_PICKPOCK,
    DUNGEON_FACILITIES_ENTRANCE,
    DUNGEON_FACILITIES_LADDER,
    DUNGEON_FACILITIES_TREASURE,
    DUNGEON_INTER_ROOM,
    DUNGEON_INTER_ENTRANCE,
    DUNGEON_INTER_LADDER,
    DUNGEON_INTER_TREASURE,
    DUNGEON_LOCK,
    HEROES_GROO_PROFILE_HEAD,
    HEROES_GROO_PROFILE_TAIL,
    HEROES_GROO_PROFILE_TORSO,
    HEROES_GROO_PROFILE_PELVIS,
    HEROES_GROO_PROFILE_ARM1A,
    HEROES_GROO_PROFILE_ARM1B,
    HEROES_GROO_PROFILE_HAND1,
    HEROES_GROO_PROFILE_ARM2A,
    HEROES_GROO_PROFILE_ARM2B,
    HEROES_GROO_PROFILE_HAND2,
    HEROES_GROO_PROFILE_LEG1A,
    HEROES_GROO_PROFILE_LEG1B,
    HEROES_GROO_PROFILE_LEG1C,
    HEROES_GROO_PROFILE_FOOT1,
    HEROES_GROO_PROFILE_LEG2A,
    HEROES_GROO_PROFILE_LEG2B,
    HEROES_GROO_PROFILE_LEG2C,
    HEROES_GROO_PROFILE_FOOT2,
};

enum class AnimationID : uint8
{
    DEFAULT,
    JUMPINGTOASTS,
    HEROES_GROO,
};

enum class ShaderID : uint8
{
    DEFAULT,
    NONE,
    JUMPINGTOASTS_BACKGROUND,
    MENU_MAIN_BACKGROUND,
    MENU_NAME,
    NUI_HOVER,
    NUI_FOCUS,
};

enum class FontID : uint8
{
    DEFAULT,
    MONO,
    NUI,
    HORROR,
};

enum class SoundID : uint8
{
    DEFAULT,
    JUMPINGTOASTS,
    NUI_ACCEPT,
    NUI_REFUSE,
    NUI_SELECT,
};

enum class MusicID : uint8
{
    DEFAULT,
    MENU_THEME,
};
