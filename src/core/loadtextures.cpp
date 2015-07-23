#include "core/application.hpp"

#include "resources/identifiers.hpp"

void Application::loadTextures()
{
    // Default - debug texture
    s_context.textures.load(TextureID::DEFAULT, "res/tex/default.png", true);

    s_context.textures.setRepeated(TextureID::DEFAULT, true);

    // Splash-screen
    s_context.textures.load(TextureID::JUMPINGTOASTS_BACKGROUND, "res/tex/jumping-toasts/bg.png", true);
    s_context.textures.load(TextureID::JUMPINGTOASTS_JUMPING, "res/tex/jumping-toasts/jumping.png", true);
    s_context.textures.load(TextureID::JUMPINGTOASTS_TOASTS, "res/tex/jumping-toasts/toasts.png", true);
    s_context.textures.load(TextureID::JUMPINGTOASTS_TOAST, "res/tex/jumping-toasts/toast.png", true);
    s_context.textures.load(TextureID::JUMPINGTOASTS_TOASTCUT, "res/tex/jumping-toasts/toast-cut.png", true);
    s_context.textures.load(TextureID::JUMPINGTOASTS_TOASTERBACKGROUND, "res/tex/jumping-toasts/toaster-background.png", true);
    s_context.textures.load(TextureID::JUMPINGTOASTS_TOASTERBOTTOM, "res/tex/jumping-toasts/toaster-bottom.png", true);
    s_context.textures.load(TextureID::JUMPINGTOASTS_TOASTERMIDDLE, "res/tex/jumping-toasts/toaster-middle.png", true);
    s_context.textures.load(TextureID::JUMPINGTOASTS_TOASTERTOP, "res/tex/jumping-toasts/toaster-top.png", true);

    s_context.textures.setRepeated(TextureID::JUMPINGTOASTS_BACKGROUND, true);
    s_context.textures.setSmooth(TextureID::JUMPINGTOASTS_TOASTCUT, false);

    // NUI
    s_context.textures.load(TextureID::NUI_FOCUS, "res/tex/nui/focus.png");
    s_context.textures.load(TextureID::NUI_PLUS, "res/tex/nui/plus.png");
    s_context.textures.load(TextureID::NUI_MINUS, "res/tex/nui/minus.png");

    s_context.textures.setRepeated(TextureID::NUI_FOCUS, true);

    // Menu
    s_context.textures.load(TextureID::MENU_MAIN_BACKGROUND, "res/tex/menu/main/background.png");
    s_context.textures.load(TextureID::MENU_MAIN_LOGO, "res/tex/menu/main/logo.png", true);

    // Dungeon scene
    s_context.textures.load(TextureID::DUNGEON_SCENE_FRONT,     "res/tex/dungeon/scene/front.png");
    s_context.textures.load(TextureID::DUNGEON_SCENE_CLOSE,     "res/tex/dungeon/scene/close.png");
    s_context.textures.load(TextureID::DUNGEON_SCENE_MIDDLE,    "res/tex/dungeon/scene/middle.png");
    s_context.textures.load(TextureID::DUNGEON_SCENE_FAR,       "res/tex/dungeon/scene/far.png");
    s_context.textures.load(TextureID::DUNGEON_SCENE_HORIZON,   "res/tex/dungeon/scene/horizon.png");
    s_context.textures.load(TextureID::DUNGEON_SCENE_SKY,       "res/tex/dungeon/scene/sky.png");

    // Dungeon panel
    s_context.textures.load(TextureID::DUNGEON_PANEL_BACKGROUND, "res/tex/dungeon/panel/background.png");
    s_context.textures.load(TextureID::DUNGEON_PANEL_SWITCH, "res/tex/dungeon/panel/switch.png");
    s_context.textures.load(TextureID::DUNGEON_PANEL_MONSTERS, "res/tex/dungeon/panel/monsters.png");
    s_context.textures.load(TextureID::DUNGEON_PANEL_TRAPS, "res/tex/dungeon/panel/traps.png");
    s_context.textures.load(TextureID::DUNGEON_PANEL_FACILITIES, "res/tex/dungeon/panel/facilities.png");

    // Dungeon inter
    s_context.textures.load(TextureID::DUNGEON_INTER_VOID_INNER_WALL, "res/tex/dungeon/inter/void_inner_wall.png");
    s_context.textures.load(TextureID::DUNGEON_INTER_VOID_RIGHT_WALL, "res/tex/dungeon/inter/void_right_wall.png");
    s_context.textures.load(TextureID::DUNGEON_INTER_VOID_FLOOR, "res/tex/dungeon/inter/void_floor.png");
    s_context.textures.load(TextureID::DUNGEON_INTER_VOID_EAST_TRANSITION, "res/tex/dungeon/inter/void_east_transition.png");
    s_context.textures.load(TextureID::DUNGEON_INTER_VOID_WEST_TRANSITION, "res/tex/dungeon/inter/void_west_transition.png");
    s_context.textures.load(TextureID::DUNGEON_INTER_VOID_SOUTH_TRANSITION, "res/tex/dungeon/inter/void_south_transition.png");
    s_context.textures.load(TextureID::DUNGEON_INTER_INNER_WALL, "res/tex/dungeon/inter/inner_wall.png");
    s_context.textures.load(TextureID::DUNGEON_INTER_OUTER_WALL, "res/tex/dungeon/inter/outer_wall.png");
    s_context.textures.load(TextureID::DUNGEON_INTER_RIGHT_WALL, "res/tex/dungeon/inter/right_wall.png");
    s_context.textures.load(TextureID::DUNGEON_INTER_FLOOR, "res/tex/dungeon/inter/floor.png");
    s_context.textures.load(TextureID::DUNGEON_INTER_ENTRANCE, "res/tex/dungeon/inter/entrance.png");
    s_context.textures.load(TextureID::DUNGEON_INTER_LADDER, "res/tex/dungeon/inter/ladder.png");
    s_context.textures.load(TextureID::DUNGEON_INTER_TREASURE, "res/tex/dungeon/inter/treasure.png");

    s_context.textures.setRepeated(TextureID::DUNGEON_INTER_OUTER_WALL, true);

    // Dungeon faciilities
    s_context.textures.load(TextureID::DUNGEON_FACILITIES_LADDER_ICON, "res/tex/dungeon/facilities/ladder/icon.png");
    s_context.textures.load(TextureID::DUNGEON_FACILITIES_LADDER_MAIN, "res/tex/dungeon/facilities/ladder/main.png");
    s_context.textures.load(TextureID::DUNGEON_FACILITIES_LADDER_EXIT_MAIN, "res/tex/dungeon/facilities/ladder/exit_main.png");
    s_context.textures.load(TextureID::DUNGEON_FACILITIES_LADDER_EXIT_END, "res/tex/dungeon/facilities/ladder/exit_end.png");

    s_context.textures.load(TextureID::DUNGEON_FACILITIES_TREASURE_ICON, "res/tex/dungeon/facilities/treasure/icon.png");
    // TODO Have a way to load all file from the directory, without having an associated TextureID (not used with SCML animation).
    s_context.textures.load(TextureID::DUNGEON_FACILITIES_TREASURE_BASE, "res/tex/dungeon/facilities/treasure/base.png", true);
    s_context.textures.load(TextureID::DUNGEON_FACILITIES_TREASURE_OPEN, "res/tex/dungeon/facilities/treasure/open.png", true);
    s_context.textures.load(TextureID::DUNGEON_FACILITIES_TREASURE_CLOSE, "res/tex/dungeon/facilities/treasure/close.png", true);
    s_context.textures.load(TextureID::DUNGEON_FACILITIES_TREASURE_HALF, "res/tex/dungeon/facilities/treasure/half.png", true);
    s_context.textures.load(TextureID::DUNGEON_FACILITIES_TREASURE_FULL, "res/tex/dungeon/facilities/treasure/full.png", true);

    s_context.textures.load(TextureID::DUNGEON_FACILITIES_ENTRANCE_ICON, "res/tex/dungeon/facilities/entrance/icon.png");
    s_context.textures.load(TextureID::DUNGEON_FACILITIES_ENTRANCE_FRAME, "res/tex/dungeon/facilities/entrance/frame.png", true);
    s_context.textures.load(TextureID::DUNGEON_FACILITIES_ENTRANCE_DOOR_OPEN, "res/tex/dungeon/facilities/entrance/door_open.png", true);
    s_context.textures.load(TextureID::DUNGEON_FACILITIES_ENTRANCE_DOOR_CLOSE, "res/tex/dungeon/facilities/entrance/door_close.png", true);

    // Dungeon traps
    s_context.textures.load(TextureID::DUNGEON_TRAPS_PICKPOCK_ICON, "res/tex/dungeon/traps/pickpock/icon.png");
    s_context.textures.load(TextureID::DUNGEON_TRAPS_PICKPOCK_TRAPDOOR_FRAME, "res/tex/dungeon/traps/pickpock/trapdoor_frame.png", true);
    s_context.textures.load(TextureID::DUNGEON_TRAPS_PICKPOCK_DOOR_OPEN, "res/tex/dungeon/traps/pickpock/door_close.png", true);
    s_context.textures.load(TextureID::DUNGEON_TRAPS_PICKPOCK_DOOR_CLOSE, "res/tex/dungeon/traps/pickpock/door_open.png", true);
    s_context.textures.load(TextureID::DUNGEON_TRAPS_PICKPOCK_ROPE_DOWN, "res/tex/dungeon/traps/pickpock/rope_down.png", true);
    s_context.textures.load(TextureID::DUNGEON_TRAPS_PICKPOCK_ROPE_UP, "res/tex/dungeon/traps/pickpock/rope_up.png", true);
    s_context.textures.load(TextureID::DUNGEON_TRAPS_PICKPOCK_LOOP_DOWN, "res/tex/dungeon/traps/pickpock/loop_down.png", true);
    s_context.textures.load(TextureID::DUNGEON_TRAPS_PICKPOCK_LOOP_UP, "res/tex/dungeon/traps/pickpock/loop_up.png", true);

    // Dungeon lock
    s_context.textures.load(TextureID::DUNGEON_LOCK, "res/tex/dungeon/panel/lock.png");

    // Heroes
    s_context.textures.load(TextureID::HEROES_GROO_PROFILE_HEAD, "res/tex/heroes/groo/profile/head.png", true);
    s_context.textures.load(TextureID::HEROES_GROO_PROFILE_TAIL, "res/tex/heroes/groo/profile/tail.png", true);
    s_context.textures.load(TextureID::HEROES_GROO_PROFILE_TORSO, "res/tex/heroes/groo/profile/torso.png", true);
    s_context.textures.load(TextureID::HEROES_GROO_PROFILE_PELVIS, "res/tex/heroes/groo/profile/pelvis.png", true);
    s_context.textures.load(TextureID::HEROES_GROO_PROFILE_ARM1A, "res/tex/heroes/groo/profile/arm1a.png", true);
    s_context.textures.load(TextureID::HEROES_GROO_PROFILE_ARM1B, "res/tex/heroes/groo/profile/arm1b.png", true);
    s_context.textures.load(TextureID::HEROES_GROO_PROFILE_HAND1, "res/tex/heroes/groo/profile/hand1.png", true);
    s_context.textures.load(TextureID::HEROES_GROO_PROFILE_ARM2A, "res/tex/heroes/groo/profile/arm2a.png", true);
    s_context.textures.load(TextureID::HEROES_GROO_PROFILE_ARM2B, "res/tex/heroes/groo/profile/arm2b.png", true);
    s_context.textures.load(TextureID::HEROES_GROO_PROFILE_HAND2, "res/tex/heroes/groo/profile/hand2.png", true);
    s_context.textures.load(TextureID::HEROES_GROO_PROFILE_LEG1A, "res/tex/heroes/groo/profile/leg1a.png", true);
    s_context.textures.load(TextureID::HEROES_GROO_PROFILE_LEG1B, "res/tex/heroes/groo/profile/leg1b.png", true);
    s_context.textures.load(TextureID::HEROES_GROO_PROFILE_LEG1C, "res/tex/heroes/groo/profile/leg1c.png", true);
    s_context.textures.load(TextureID::HEROES_GROO_PROFILE_FOOT1, "res/tex/heroes/groo/profile/foot1.png", true);
    s_context.textures.load(TextureID::HEROES_GROO_PROFILE_LEG2A, "res/tex/heroes/groo/profile/leg2a.png", true);
    s_context.textures.load(TextureID::HEROES_GROO_PROFILE_LEG2B, "res/tex/heroes/groo/profile/leg2b.png", true);
    s_context.textures.load(TextureID::HEROES_GROO_PROFILE_LEG2C, "res/tex/heroes/groo/profile/leg2c.png", true);
    s_context.textures.load(TextureID::HEROES_GROO_PROFILE_FOOT2, "res/tex/heroes/groo/profile/foot2.png", true);
}
