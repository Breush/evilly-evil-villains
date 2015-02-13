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
    const sf::Vector2f& resolution = Application::context().resolution;
    float maxSize = std::max(resolution.x, resolution.y);

    // Background
    const auto& texture = Application::context().textures.get(Textures::MENU_BG);
    const auto& textureSize = texture.getSize();
    m_bgSprite.setTexture(texture);
    m_bgSprite.setScale(maxSize / textureSize.x, maxSize / textureSize.y);
    m_bgSprite.setPosition(sf::vsub(resolution, maxSize) / 2.f);
    m_bgSprite.setTexture(texture);

    // Shaders
    m_bgShader = &Application::context().shaders.get(Shaders::MENU_BG);

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
    m_reactImage.setImageShader(Shaders::MENU_NAME);
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

void MenuMainState::draw()
{
    auto& window = Application::context().window;
    window.setView(Application::context().views.get(Views::DEFAULT));

    // Animated background and menu
    // TODO Include background as entity
    window.draw(m_bgSprite, m_bgShader);

    // Should not exists
    State::draw();
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
