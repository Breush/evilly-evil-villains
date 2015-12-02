#include "core/application.hpp"

#include "states/quit.hpp"
#include "states/terminal.hpp"
#include "states/splashscreen.hpp"
#include "states/menu/main.hpp"
#include "states/menu/config.hpp"
#include "states/menu/selectworld.hpp"
#include "states/menu/createworld.hpp"
#include "states/menu/managevillains.hpp"
#include "states/menu/createvillain.hpp"
#include "states/game/dcb.hpp"
#include "states/game/dungeondesign.hpp"
#include "states/game/pause.hpp"
#include "states/game/over.hpp"
#include "states/hub/main.hpp"
#include "states/hub/market.hpp"

void Application::loadStates()
{
    m_stateStack.registerState<states::Quit>(StateID::QUIT);
    m_stateStack.registerState<states::Terminal>(StateID::TERMINAL);
    m_stateStack.registerState<states::SplashScreen>(StateID::SPLASHSCREEN);

    m_stateStack.registerState<states::MenuMain>(StateID::MENU_MAIN);
    m_stateStack.registerState<states::MenuConfig>(StateID::MENU_CONFIG);
    m_stateStack.registerState<states::MenuSelectWorld>(StateID::MENU_SELECTWORLD);
    m_stateStack.registerState<states::MenuCreateWorld>(StateID::MENU_CREATEWORLD);
    m_stateStack.registerState<states::MenuManageVillains>(StateID::MENU_MANAGEVILLAINS);
    m_stateStack.registerState<states::MenuCreateVillain>(StateID::MENU_CREATEVILLAIN);

    m_stateStack.registerState<states::GameDCB>(StateID::GAME_DCB);
    m_stateStack.registerState<states::GameDungeonDesign>(StateID::GAME_DUNGEON_DESIGN);
    m_stateStack.registerState<states::GamePause>(StateID::GAME_PAUSE);
    m_stateStack.registerState<states::GameOver>(StateID::GAME_OVER);

    m_stateStack.registerState<states::HubMain>(StateID::HUB_MAIN);
    m_stateStack.registerState<states::HubMarket>(StateID::HUB_MARKET);
}

