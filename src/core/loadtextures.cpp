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

    s_context.textures.setRepeated(TextureID::NUI_FOCUS, true);

    // Menu
    s_context.textures.load(TextureID::MENU_BACKGROUND, "res/tex/menu/bg.png");
    s_context.textures.load(TextureID::MENU_NAME, "res/tex/menu/name.png");

    // Dungeon
    s_context.textures.load(TextureID::DUNGEON_SCENE_GRASSYHILLS_BACK, "res/tex/dungeon/scene/grassyhills_back.png");
    s_context.textures.load(TextureID::DUNGEON_SCENE_GRASSYHILLS_MIDDLE, "res/tex/dungeon/scene/grassyhills_middle.png");
    s_context.textures.load(TextureID::DUNGEON_SCENE_GRASSYHILLS_FRONT, "res/tex/dungeon/scene/grassyhills_front.png");

    s_context.textures.load(TextureID::DUNGEON_PANEL_BACKGROUND, "res/tex/dungeon/panel/background.png");
    s_context.textures.load(TextureID::DUNGEON_PANEL_SWITCH, "res/tex/dungeon/panel/switch.png");
    s_context.textures.load(TextureID::DUNGEON_PANEL_MONSTERS, "res/tex/dungeon/panel/monsters.png");
    s_context.textures.load(TextureID::DUNGEON_PANEL_TRAPS, "res/tex/dungeon/panel/traps.png");
    s_context.textures.load(TextureID::DUNGEON_PANEL_FACILITIES, "res/tex/dungeon/panel/facilities.png");

    s_context.textures.load(TextureID::DUNGEON_INTER_ROOM, "res/tex/dungeon/inter/room.png");
    s_context.textures.load(TextureID::DUNGEON_INTER_ENTRANCE, "res/tex/dungeon/inter/entrance.png");
    s_context.textures.load(TextureID::DUNGEON_INTER_LADDER, "res/tex/dungeon/inter/ladder.png");
    s_context.textures.load(TextureID::DUNGEON_INTER_TREASURE, "res/tex/dungeon/inter/treasure.png");

    s_context.textures.load(TextureID::DUNGEON_FACILITIES_ENTRANCE, "res/tex/dungeon/facilities/entrance.png");
    s_context.textures.load(TextureID::DUNGEON_FACILITIES_LADDER, "res/tex/dungeon/facilities/ladder.png");
    s_context.textures.load(TextureID::DUNGEON_FACILITIES_TREASURE, "res/tex/dungeon/facilities/treasure.png");

    s_context.textures.load(TextureID::DUNGEON_LOCK, "res/tex/dungeon/panel/lock.png");
}
