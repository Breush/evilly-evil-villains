#include "core/application.hpp"

#include "states/quit.hpp"
#include "states/splashscreen.hpp"
#include "states/menu/main.hpp"
#include "states/menu/selectworld.hpp"
#include "states/menu/createworld.hpp"
#include "states/game/dungeondesign.hpp"
#include "states/game/pause.hpp"

void Application::loadStates()
{
    m_stateStack.registerState<QuitState>(StateID::QUIT);
    m_stateStack.registerState<SplashScreenState>(StateID::SPLASHSCREEN);

    m_stateStack.registerState<MenuMainState>(StateID::MENU_MAIN);
    m_stateStack.registerState<MenuSelectWorldState>(StateID::MENU_SELECTWORLD);
    m_stateStack.registerState<MenuCreateWorldState>(StateID::MENU_CREATEWORLD);

    m_stateStack.registerState<GameDungeonDesignState>(StateID::GAME_DUNGEON_DESIGN);
    m_stateStack.registerState<GamePauseState>(StateID::GAME_PAUSE);
}

