#include "core/application.hpp"

void Application::loadTextures()
{
    // Default - debug texture
    s_context.textures.load(Textures::DEFAULT, "res/tex/default.png", true);

    s_context.textures.setRepeated(Textures::DEFAULT, true);

    // Splash-screen
    s_context.textures.load(Textures::JUMPINGTOASTS_BG, "res/tex/jumping-toasts/bg.png", true);
    s_context.textures.load(Textures::JUMPINGTOASTS_JUMPING, "res/tex/jumping-toasts/jumping.png", true);
    s_context.textures.load(Textures::JUMPINGTOASTS_TOASTS, "res/tex/jumping-toasts/toasts.png", true);
    s_context.textures.load(Textures::JUMPINGTOASTS_TOAST, "res/tex/jumping-toasts/toast.png", true);
    s_context.textures.load(Textures::JUMPINGTOASTS_TOASTCUT, "res/tex/jumping-toasts/toast-cut.png", true);
    s_context.textures.load(Textures::JUMPINGTOASTS_TOASTERBACKGROUND, "res/tex/jumping-toasts/toaster-background.png", true);
    s_context.textures.load(Textures::JUMPINGTOASTS_TOASTERBOTTOM, "res/tex/jumping-toasts/toaster-bottom.png", true);
    s_context.textures.load(Textures::JUMPINGTOASTS_TOASTERMIDDLE, "res/tex/jumping-toasts/toaster-middle.png", true);
    s_context.textures.load(Textures::JUMPINGTOASTS_TOASTERTOP, "res/tex/jumping-toasts/toaster-top.png", true);

    s_context.textures.setRepeated(Textures::JUMPINGTOASTS_BG, true);
    s_context.textures.setSmooth(Textures::JUMPINGTOASTS_TOASTCUT, false);

    // NUI
    s_context.textures.load(Textures::NUI_FOCUS, "res/tex/nui/focus.png");

    s_context.textures.setRepeated(Textures::NUI_FOCUS, true);

    // Menu
    s_context.textures.load(Textures::MENU_BG, "res/tex/menu/bg.png");
    s_context.textures.load(Textures::MENU_NAME, "res/tex/menu/name.png");

    // Dungeon
    s_context.textures.load(Textures::DUNGEON_SCENE_GRASSYHILLS_BACK, "res/tex/dungeon/scene/grassyhills_back.png");
    s_context.textures.load(Textures::DUNGEON_SCENE_GRASSYHILLS_FRONT, "res/tex/dungeon/scene/grassyhills_front.png");

    s_context.textures.load(Textures::DUNGEON_PANEL_BACKGROUND, "res/tex/dungeon/panel/background.png");
    s_context.textures.load(Textures::DUNGEON_PANEL_SWITCH, "res/tex/dungeon/panel/switch.png");
    s_context.textures.load(Textures::DUNGEON_PANEL_MONSTERS, "res/tex/dungeon/panel/monsters.png");
    s_context.textures.load(Textures::DUNGEON_PANEL_TRAPS, "res/tex/dungeon/panel/traps.png");
    s_context.textures.load(Textures::DUNGEON_PANEL_FACILITIES, "res/tex/dungeon/panel/facilities.png");
    s_context.textures.load(Textures::DUNGEON_PANEL_TREASURES, "res/tex/dungeon/panel/treasures.png");

    s_context.textures.load(Textures::DUNGEON_INTER_ROOM, "res/tex/dungeon/inter/room.png");
}
