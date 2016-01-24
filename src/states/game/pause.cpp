#include "states/game/pause.hpp"

#include "core/gettext.hpp"
#include "core/application.hpp"
#include "tools/tools.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

using namespace states;

GamePause::GamePause(StateStack& stack)
    : State(stack)
{
    // Creating scene
    auto& nuiRoot = nuiLayer().root();

    // Background
    nuiRoot.attachChild(m_background);
    m_background.setFillColor(sf::Color(0, 0, 0, 230));

    // Title
    nuiRoot.attachChild(m_title);
    m_title.setPrestyle(scene::Label::Prestyle::MENU_POPUP_TITLE);
    m_title.setRelativePosition({0.5f, 0.35f});
    m_title.centerOrigin();

    // Buttons
    nuiRoot.attachChild(m_stacker);
    m_stacker.stackBack(m_continueButton, nui::Align::CENTER);
    m_stacker.stackBack(m_configButton,   nui::Align::CENTER);
    m_stacker.stackBack(m_mainMenuButton, nui::Align::CENTER);
    m_stacker.setRelativePosition({0.5f, 0.5f});
    m_stacker.centerOrigin();

    m_continueButton.setValidateCallback([this]() { stackPop(); });
    m_mainMenuButton.setValidateCallback([this]() { stackClear(StateID::MENU_MAIN); });
    m_configButton.setValidateCallback  ([this]() { stackPush(StateID::MENU_CONFIG); });

    Application::setPaused(true);
}

GamePause::~GamePause()
{
    Application::setPaused(false);
}

//-------------------//
//----- Routine -----//

bool GamePause::update(const sf::Time& dt)
{
    baseClass::update(dt);
    return false;
}

void GamePause::handleEvent(const sf::Event& event)
{
    // Back to previous state on Escape
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPop();
        return;
    }

    State::handleEvent(event);
}

void GamePause::refreshWindow(const config::WindowInfo& cWindow)
{
    baseClass::refreshWindow(cWindow);

    // Translated strings
    m_title.setText(_("Game paused"));

    m_continueButton.setText(_("Continue, continue, continue"));
    m_configButton.setText  (_("Options"));
    m_mainMenuButton.setText(_("Save and return to main menu"));

    // Background
    m_background.setSize(cWindow.resolution);
}
