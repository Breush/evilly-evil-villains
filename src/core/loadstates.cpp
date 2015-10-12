#include "core/application.hpp"

#include "states/quit.hpp"
#include "states/terminal.hpp"
#include "states/splashscreen.hpp"
#include "states/menu/main.hpp"
#include "states/menu/selectworld.hpp"
#include "states/menu/createworld.hpp"
#include "states/menu/managevillains.hpp"
#include "states/menu/createvillain.hpp"
#include "states/game/dcb.hpp"
#include "states/game/dungeondesign.hpp"
#include "states/game/pause.hpp"

void Application::loadStates()
{
    m_stateStack.registerState<states::Quit>(StateID::QUIT);
    m_stateStack.registerState<states::Terminal>(StateID::TERMINAL);
    m_stateStack.registerState<states::SplashScreen>(StateID::SPLASHSCREEN);

    m_stateStack.registerState<states::MenuMain>(StateID::MENU_MAIN);
    m_stateStack.registerState<states::MenuSelectWorld>(StateID::MENU_SELECTWORLD);
    m_stateStack.registerState<states::MenuCreateWorld>(StateID::MENU_CREATEWORLD);
    m_stateStack.registerState<states::MenuManageVillains>(StateID::MENU_MANAGEVILLAINS);
    m_stateStack.registerState<states::MenuCreateVillain>(StateID::MENU_CREATEVILLAIN);

    m_stateStack.registerState<states::GameDCB>(StateID::GAME_DCB);
    m_stateStack.registerState<states::GameDungeonDesign>(StateID::GAME_DUNGEON_DESIGN);
    m_stateStack.registerState<states::GamePause>(StateID::GAME_PAUSE);
}

