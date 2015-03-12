#include "states/menu/main.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "resources/identifiers.hpp"
#include "tools/tools.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

using namespace states;

MenuMain::MenuMain(StateStack& stack)
    : baseClass(stack)
    , m_choices{L"V", L"I", L"L", L"Y", L"S"}
{
    // During menus, enable key repeat
    Application::context().window.setKeyRepeatEnabled(true);

    const sf::Vector2f& resolution = Application::context().resolution;
    float maxSide = std::max(resolution.x, resolution.y);

    // Background
    // TODO Make a expandToView() function in sfe::Sprite to adapt to screenSize?
    const auto& textureSize = Application::context().textures.get(TextureID::MENU_BACKGROUND).getSize();
    sceneLayer(LayerID::NUI).attachChild(m_background);
    m_background.setDepth(100.f);
    m_background.setTexture(TextureID::MENU_BACKGROUND);
    m_background.setShader(ShaderID::MENU_BACKGROUND);
    m_background.setLocalScale({maxSide / textureSize.x, maxSide / textureSize.y});
    m_background.setLocalPosition(sf::vsub(resolution, maxSide) / 2.f);

    // Fonctors
    auto singlePlayer = [this]() { stackPush(StateID::MENU_SELECTWORLD); };
    auto multiPlayer = nullptr;
    auto personalization = nullptr;
    auto configuration = nullptr;
    auto quitGame = [this]() { stackClear(); };

    // Menu choice box (be sure not to mess with order)
    sceneLayer(LayerID::NUI).attachChild(m_choiceBox);
    m_choiceBox.setCentered(true);
    m_choiceBox.setLocalPosition({0.5f * resolution.x, 0.9f * resolution.y});
    m_choiceBox.add(_("Victim and alone"), singlePlayer);
    m_choiceBox.add(_("I sometimes have friends"), multiPlayer);
    m_choiceBox.add(_("Looking at who I am"), personalization);
    m_choiceBox.add(_("Yet I want to choose"), configuration);
    m_choiceBox.add(_("Someone who runs away"), quitGame);

    // Menu react image
    sceneLayer(LayerID::NUI).attachChild(m_reactImage);
    m_reactImage.setCentered(true);
    m_reactImage.setLocalPosition({0.5f * resolution.x, 0.2f * resolution.y});
    m_reactImage.setImageTexture(TextureID::MENU_NAME);
    m_reactImage.setShader(ShaderID::MENU_NAME);
    m_reactImage.setMouseLeftDeselect(false);

    // Setting callbacks
    m_reactImage.addReactFromFile("res/tex/menu/name.xml");
    m_reactImage.setReactCallback(m_choices[0], singlePlayer);
    m_reactImage.setReactCallback(m_choices[1], multiPlayer);
    m_reactImage.setReactCallback(m_choices[2], personalization);
    m_reactImage.setReactCallback(m_choices[3], configuration);
    m_reactImage.setReactCallback(m_choices[4], quitGame);

    // Menu theme
    Application::context().musics.play(MusicID::MENU_THEME);
    Application::context().musics.setVolume(75);
}

bool MenuMain::update(const sf::Time& dt)
{
    // Checking if choiceBox changed
    if (m_choiceBox.choiceChanged()) {
        m_reactImage.setActiveReact(m_choices[m_choiceBox.selectedChoice()]);
    }
    // Checking if reactImage changed
    else if (m_reactImage.reactChanged()) {
        for (uint i = 0; i < m_choices.size(); ++i) {
            if (m_choices[i] == m_reactImage.getReact()) {
                m_choiceBox.selectChoice(i);
                break;
            }
        }
    }

    return State::update(dt);
}

bool MenuMain::handleEvent(const sf::Event& event)
{
    // Escape opens quit screen
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPush(StateID::QUIT);
        return false;
    }

    return State::handleEvent(event);
}
