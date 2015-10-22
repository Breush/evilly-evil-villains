#include "states/menu/config.hpp"

#include "core/gettext.hpp"
#include "context/villains.hpp"
#include "tools/tools.hpp"
#include "tools/string.hpp"

using namespace states;

MenuConfig::MenuConfig(StateStack& stack)
    : baseClass(stack)
{
    // Creating scene
    auto& nuiRoot = nuiLayer().root();
    const auto& nuiSize = nuiLayer().size();

    // Background
    nuiRoot.attachChild(m_background);
    m_background.setDepth(100.f);
    m_background.setFillColor(sf::Color(0, 0, 0, 230));
    m_background.setSize(nuiSize);

    // Title
    nuiRoot.attachChild(m_title);
    m_title.setText(_("Configuration"));
    m_title.setPrestyle(scene::Label::Prestyle::MENU_TITLE);
    m_title.setRelativePosition({0.5f, 0.05f});
    m_title.centerOrigin();

    // TODO Config content:
    // - General/UI:
    //      - Language
    //      - Scrolling factor
    //      - Zoom speed
    //      - UI Size
    //      - Font size factor
    // - Graphics:
    //      - FULLSCREEN ON/OFF
    //      - VSync ON / OFF
    //      - Resolutions (between those available for full-screen)
    // - Audio:
    //      - General volume
    //      - Volume musics
    //      - Volume sounds

}

//-------------------//
//----- Routine -----//

void MenuConfig::handleEvent(const sf::Event& event)
{
    // Escape opens quit screen
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPop();
        return;
    }

    State::handleEvent(event);
}

