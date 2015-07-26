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
    NUI_PLUS,
    NUI_MINUS,
    MENU_MAIN_BACKGROUND,
    MENU_MAIN_LOGO,
    DUNGEON_SCENE_GRASSYHILLS_BACK,
    DUNGEON_SCENE_GRASSYHILLS_MIDDLE,
    DUNGEON_SCENE_GRASSYHILLS_FRONT,
    DUNGEON_SCENE_FRONT,
    DUNGEON_SCENE_CLOSE,
    DUNGEON_SCENE_MIDDLE,
    DUNGEON_SCENE_FAR,
    DUNGEON_SCENE_HORIZON,
    DUNGEON_SCENE_SKY,
    DUNGEON_SIDEBAR_BACKGROUND,
    DUNGEON_SIDEBAR_TAB_MONSTERS,
    DUNGEON_SIDEBAR_TAB_TRAPS,
    DUNGEON_SIDEBAR_TAB_FACILITIES,
    DUNGEON_MONSTERS_CREEPIM,
    DUNGEON_TRAPS_PICKPOCK_ICON,
    DUNGEON_TRAPS_PICKPOCK_TRAPDOOR_FRAME,
    DUNGEON_TRAPS_PICKPOCK_DOOR_OPEN,
    DUNGEON_TRAPS_PICKPOCK_DOOR_CLOSE,
    DUNGEON_TRAPS_PICKPOCK_ROPE_DOWN,
    DUNGEON_TRAPS_PICKPOCK_ROPE_UP,
    DUNGEON_TRAPS_PICKPOCK_LOOP_DOWN,
    DUNGEON_TRAPS_PICKPOCK_LOOP_UP,
    DUNGEON_FACILITIES_ENTRANCE_ICON,
    DUNGEON_FACILITIES_ENTRANCE_FRAME,
    DUNGEON_FACILITIES_ENTRANCE_DOOR_OPEN,
    DUNGEON_FACILITIES_ENTRANCE_DOOR_CLOSE,
    DUNGEON_FACILITIES_LADDER_ICON,
    DUNGEON_FACILITIES_LADDER_MAIN,
    DUNGEON_FACILITIES_LADDER_EXIT_MAIN,
    DUNGEON_FACILITIES_LADDER_EXIT_END,
    DUNGEON_FACILITIES_TREASURE_ICON,
    DUNGEON_FACILITIES_TREASURE_BASE,
    DUNGEON_FACILITIES_TREASURE_OPEN,
    DUNGEON_FACILITIES_TREASURE_CLOSE,
    DUNGEON_FACILITIES_TREASURE_HALF,
    DUNGEON_FACILITIES_TREASURE_FULL,
    DUNGEON_INTER_VOID_INNER_WALL,
    DUNGEON_INTER_VOID_RIGHT_WALL,
    DUNGEON_INTER_VOID_FLOOR,
    DUNGEON_INTER_VOID_EAST_TRANSITION,
    DUNGEON_INTER_VOID_WEST_TRANSITION,
    DUNGEON_INTER_VOID_SOUTH_TRANSITION,
    DUNGEON_INTER_INNER_WALL,
    DUNGEON_INTER_OUTER_WALL,
    DUNGEON_INTER_RIGHT_WALL,
    DUNGEON_INTER_FLOOR,
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
    DUNGEON_FACILITIES_ENTRANCE,
    DUNGEON_FACILITIES_TREASURE,
    DUNGEON_TRAPS_PICKPOCK,
};

enum class ShaderID : uint8
{
    DEFAULT,
    NONE,
    JUMPINGTOASTS_BACKGROUND,
    MENU_MAIN_BACKGROUND,
    MENU_MAIN_LOGO,
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
