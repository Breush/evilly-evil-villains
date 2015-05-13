#include "states/menu/main.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "core/version.hpp"
#include "resources/identifiers.hpp"
#include "tools/vector.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

using namespace states;

MenuMain::MenuMain(StateStack& stack)
    : baseClass(stack)
    , m_choices{L"V", L"I", L"L", L"Y", L"S"}
{
    // During menus, enable key repeat
    Application::context().window.setKeyRepeatEnabled(true);

    // Menu theme
    Application::context().musics.play(MusicID::MENU_THEME);
    Application::context().musics.setVolume(75);

    // Creating scene
    auto& nuiRoot = nuiLayer().root();
    const auto& nuiSize = nuiLayer().size();
    float maxSide = std::max(nuiSize.x, nuiSize.y);

    // Background
    const auto& textureSize = Application::context().textures.get(TextureID::MENU_BACKGROUND).getSize();
    nuiRoot.attachChild(m_background);
    m_background.setDepth(100.f);
    m_background.setTexture(TextureID::MENU_BACKGROUND);
    m_background.setShader(ShaderID::MENU_BACKGROUND);
    m_background.setLocalScale(maxSide / sf::v2f(textureSize));
    m_background.setLocalPosition((nuiSize - maxSide) / 2.f);

    // Copyright label
    nuiRoot.attachChild(m_copyrightLabel);
    m_copyrightLabel.setText(EEV_COPYRIGHT_SHORT_WS);
    m_copyrightLabel.setPrestyle(sfe::Label::Prestyle::MENU_SOBER_LIGHT);
    m_copyrightLabel.setLocalPosition({0.f, nuiSize.y - m_copyrightLabel.size().y});

    // Version label
    nuiRoot.attachChild(m_versionLabel);
    std::wstringstream versionString;
    versionString << EEV_VERSION_SWEET_NAME_WS << L" ";
    versionString << EEV_VERSION_MAJ << L"." << EEV_VERSION_MIN << L"-" << EEV_VERSION_REV_WS;
    m_versionLabel.setText(versionString.str());
    m_versionLabel.setPrestyle(sfe::Label::Prestyle::MENU_SOBER);
    m_versionLabel.setLocalPosition(nuiSize - m_versionLabel.size());

    // Functors
    auto singlePlayer = [this]() { stackPush(StateID::MENU_SELECTWORLD); };
    auto multiPlayer = nullptr;
    auto villains = [this]() { stackPush(StateID::MENU_VILLAINS); };
    auto configuration = nullptr;
    auto quitGame = [this]() { stackClear(); };

    // Menu choice box (be sure not to mess with order)
    nuiRoot.attachChild(m_choiceBox);
    m_choiceBox.centerOrigin();
    m_choiceBox.setRelativePosition({0.5f, 0.9f});
    m_choiceBox.add(_("Victim and alone"), singlePlayer);
    m_choiceBox.add(_("I sometimes have friends"), multiPlayer);
    m_choiceBox.add(_("Looking at who I am"), villains);
    m_choiceBox.add(_("Yet I want to choose"), configuration);
    m_choiceBox.add(_("Someone who runs away"), quitGame);

    // Menu react image
    nuiRoot.attachChild(m_reactImage);
    m_reactImage.centerOrigin();
    m_reactImage.setRelativePosition({0.5f, 0.2f});
    m_reactImage.setImageTexture(TextureID::MENU_NAME);
    m_reactImage.setShader(ShaderID::MENU_NAME);
    m_reactImage.setMouseLeftDeselect(false);

    // Setting callbacks
    m_reactImage.addReactFromFile("res/tex/menu/name.xml");
    m_reactImage.setReactCallback(m_choices[0], singlePlayer);
    m_reactImage.setReactCallback(m_choices[1], multiPlayer);
    m_reactImage.setReactCallback(m_choices[2], villains);
    m_reactImage.setReactCallback(m_choices[3], configuration);
    m_reactImage.setReactCallback(m_choices[4], quitGame);
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
