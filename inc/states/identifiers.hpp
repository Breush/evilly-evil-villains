#pragma once

#include "tools/int.hpp"

enum class StateID : uint8
{
    NONE,
    QUIT,
    TERMINAL,
    SPLASHSCREEN,
    MENU_MAIN,
    MENU_CONFIG,
    MENU_SELECTWORLD,
    MENU_CREATEWORLD,
    MENU_MANAGEVILLAINS,
    MENU_CREATEVILLAIN,
    MENU_NEWGAME,
    GAME_DCB,
    GAME_DUNGEON_DESIGN,
    GAME_PAUSE,
    GAME_OVER,
    HUB_MAIN,
    HUB_INN,
    HUB_MARKET,
};
