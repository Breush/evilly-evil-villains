#include "states/menu/main.hpp"

#include "core/gettext.hpp"
#include "core/application.hpp"
#include "tools/tools.hpp"
#include "resources/holder.hpp"
#include "resources/musicplayer.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>


MenuMainState::MenuMainState(StateStack& stack)
    : baseClass(stack)
    , m_bgRotAngle(0.f)
{
    const sf::Vector2f& viewSize = Application::context().resolution;

    // Background
    sf::Texture& texture = Application::context().textures.get(Textures::MENU_BG);
    m_bgSprite.setTexture(texture);
    m_bgSprite.setPosition((viewSize - sf::Vector2f(texture.getSize())) / 2.f);

    // Shaders
    m_bgShader = &Application::context().shaders.get(Shaders::MENU_BG);

    // Choices
    m_choices.push_back("V");
    m_choices.push_back("I");
    m_choices.push_back("L");
    m_choices.push_back("Y");
    m_choices.push_back("S");

    // Fonctors
    std::function<void()> singlePlayer = [this]() {
        stackPush(States::MENU_SELECTWORLD);
    };
    std::function<void()> multiPlayer = nullptr;
    std::function<void()> personalization = nullptr;
    std::function<void()> configuration = nullptr;
    std::function<void()> quitGame = [this]() {
        stackClear();
    };

    // Menu choice box (be sure not to mess with order)
    m_uiCore.add(&m_choiceBox);
    m_choiceBox.setCentered(true);
    m_choiceBox.setPosition(0.5f * viewSize.x, 0.9f * viewSize.y);
    m_choiceBox.add(_("Victim and alone"), singlePlayer);
    m_choiceBox.add(_("I sometimes have friends"), multiPlayer);
    m_choiceBox.add(_("Looking at who I am"), personalization);
    m_choiceBox.add(_("Yet I want to choose"), configuration);
    m_choiceBox.add(_("Someone who runs away"), quitGame);

    // Menu react image
    m_uiCore.add(&m_reactImage);
    m_reactImage.setCentered(true);
    m_reactImage.setFocusable(false);
    m_reactImage.setPosition(0.5f * viewSize.x, 0.2f * viewSize.y);
    m_reactImage.setImageTexture(Textures::MENU_NAME);
    m_reactImage.setImageShader(Shaders::MENU_NAME);
    m_reactImage.setMouseLeftDeselect(false);
    // TODO Implement an XML Holder ?
    m_reactImage.addReactFromFile("res/tex/menu/name.xml");
    m_reactImage.setReactCallback(m_choices[0], singlePlayer);
    m_reactImage.setReactCallback(m_choices[1], multiPlayer);
    m_reactImage.setReactCallback(m_choices[2], personalization);
    m_reactImage.setReactCallback(m_choices[3], configuration);
    m_reactImage.setReactCallback(m_choices[4], quitGame);

    // Menu theme
    Application::context().music.play(Music::MENU_THEME);
    Application::context().music.setVolume(75);
}

void MenuMainState::draw()
{
    auto& window = Application::context().window;

    // Animated background and menu
    window.draw(m_bgSprite, m_bgShader);
    window.draw(m_uiCore);
}

void MenuMainState::onHide()
{
    m_uiCore.forgetFocusedChild();
}

void MenuMainState::onShow()
{
    m_uiCore.rememberFocusedChild();
}

bool MenuMainState::update(const sf::Time& dt)
{
    // Checking if choiceBox changed
    if (m_choiceBox.status()) {
        m_reactImage.setActiveReact(m_choices[m_choiceBox.choice()]);
    }
    // Checking if reactImage changed
    else if (m_reactImage.status()) {
        for (uint i = 0; i < m_choices.size(); ++i) {
            if (m_choices[i] == m_reactImage.getReact()) {
                m_choiceBox.setChoice(i);
                break;
            }
        }
    }

    m_uiCore.update(dt);

    return true;
}

bool MenuMainState::handleEvent(const sf::Event& event)
{
    // Escape quits current state (and game)
    if (event.type == sf::Event::KeyPressed
            && event.key.code == sf::Keyboard::Escape) {
        stackPop();
        return false;
    }

    // Let ui core handle events
    m_uiCore.handleEvent(event);
    return false;
}

void MenuMainState::refresh()
{
    m_uiCore.refresh();
}
