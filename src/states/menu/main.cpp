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
{
    const sf::Vector2f& resolution = Application::context().resolution;
    float maxSide = std::max(resolution.x, resolution.y);

    // Background - TODO Make a expandToView() function in sfe::Sprite to adapt to screenSize?
    const auto& textureSize = Application::context().textures.get(Textures::MENU_BACKGROUND).getSize();
    sceneLayer(Layers::NUI).attachChild(m_background);
    m_background.setDepth(100.f);
    m_background.setTexture(Textures::MENU_BACKGROUND);
    m_background.setShader(Shaders::MENU_BACKGROUND);
    m_background.setLocalScale({maxSide / textureSize.x, maxSide / textureSize.y});
    m_background.setLocalPosition(sf::vsub(resolution, maxSide) / 2.f);

    // Choices
    m_choices.push_back(L"V");
    m_choices.push_back(L"I");
    m_choices.push_back(L"L");
    m_choices.push_back(L"Y");
    m_choices.push_back(L"S");

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
    sceneLayer(Layers::NUI).attachChild(m_choiceBox);
    m_choiceBox.setCentered(true);
    m_choiceBox.setLocalPosition({0.5f * resolution.x, 0.9f * resolution.y});
    m_choiceBox.add(_("Victim and alone"), singlePlayer);
    m_choiceBox.add(_("I sometimes have friends"), multiPlayer);
    m_choiceBox.add(_("Looking at who I am"), personalization);
    m_choiceBox.add(_("Yet I want to choose"), configuration);
    m_choiceBox.add(_("Someone who runs away"), quitGame);

    // Menu react image
    sceneLayer(Layers::NUI).attachChild(m_reactImage);
    m_reactImage.setCentered(true);
    m_reactImage.setLocalPosition({0.5f * resolution.x, 0.2f * resolution.y});
    m_reactImage.setImageTexture(Textures::MENU_NAME);
    m_reactImage.setShader(Shaders::MENU_NAME);
    m_reactImage.setMouseLeftDeselect(false);

    // Setting callbacks
    m_reactImage.addReactFromFile("res/tex/menu/name.xml");
    m_reactImage.setReactCallback(m_choices[0], singlePlayer);
    m_reactImage.setReactCallback(m_choices[1], multiPlayer);
    m_reactImage.setReactCallback(m_choices[2], personalization);
    m_reactImage.setReactCallback(m_choices[3], configuration);
    m_reactImage.setReactCallback(m_choices[4], quitGame);

    // Menu theme
    Application::context().musics.play(Musics::MENU_THEME);
    Application::context().musics.setVolume(75);
}

bool MenuMainState::update(const sf::Time& dt)
{
    // Checking if choiceBox changed
    if (m_choiceBox.choiceChanged()) {
        m_reactImage.setActiveReact(m_choices[m_choiceBox.choice()]);
    }
    // Checking if reactImage changed
    else if (m_reactImage.reactChanged()) {
        for (uint i = 0; i < m_choices.size(); ++i) {
            if (m_choices[i] == m_reactImage.getReact()) {
                m_choiceBox.setChoice(i);
                break;
            }
        }
    }

    return State::update(dt);
}

bool MenuMainState::handleEvent(const sf::Event& event)
{
    // Escape opens quit screen
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPush(States::QUIT);
        return false;
    }

    return State::handleEvent(event);
}
