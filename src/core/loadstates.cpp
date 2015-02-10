#include "core/application.hpp"

#include "states/quit.hpp"
#include "states/splashscreen.hpp"
#include "states/menu/main.hpp"
#include "states/menu/selectworld.hpp"
#include "states/game/dungeondesign.hpp"
#include "states/game/pause.hpp"

void Application::loadStates()
{
    m_stateStack.registerState<QuitState>(States::QUIT);
    m_stateStack.registerState<SplashScreenState>(States::SPLASHSCREEN);

    m_stateStack.registerState<MenuMainState>(States::MENU_MAIN);
    m_stateStack.registerState<MenuSelectWorldState>(States::MENU_SELECTWORLD);

    m_stateStack.registerState<GameDungeonDesignState>(States::GAME_DUNGEON_DESIGN);
    m_stateStack.registerState<GamePauseState>(States::GAME_PAUSE);
}

