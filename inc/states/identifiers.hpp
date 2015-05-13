#pragma once

#include "tools/int.hpp"

enum class StateID : uint8 {
    NONE,
    QUIT,
    SPLASHSCREEN,
    MENU_MAIN,
    MENU_SELECTWORLD,
    MENU_CREATEWORLD,
    MENU_VILLAINS,
    GAME_DCB,
    GAME_DUNGEON_DESIGN,
    GAME_PAUSE
};
