#include "states/menu/main.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "core/define.hpp"
#include "tools/vector.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

using namespace states;

MenuMain::MenuMain(StateStack& stack)
    : baseClass(stack)
    , m_choices{L"V", L"I", L"L", L"Y", L"S"}
{
    // Loading resources
    Application::loadTextures({"menu"});

    // During menus, enable key repeat
    Application::context().window.setKeyRepeatEnabled(true);
    Application::context().musics.play("pippin_the_hunchback");

    // Creating scene
    auto& nuiRoot = nuiLayer().root();

    // Background
    nuiRoot.attachChild(m_background);
    m_background.setDepth(100.f);
    m_background.setTexture("menu/main/background");
    // m_background.setShader("menu/background");

    // Copyright label
    nuiRoot.attachChild(m_copyrightLabel);
    m_copyrightLabel.setText(EEV_COPYRIGHT_SHORT_WS);
    m_copyrightLabel.setPrestyle(scene::Label::Prestyle::MENU_SOBER_LIGHT);
    m_copyrightLabel.setRelativePosition({0.f, 1.f});
    m_copyrightLabel.setRelativeOrigin({0.f, 1.f});

    // Version label
    nuiRoot.attachChild(m_versionLabel);
    std::wstringstream versionString;
    versionString << EEV_VERSION_SWEET_NAME_WS << L" ";
    versionString << EEV_VERSION_MAJ << L"." << EEV_VERSION_MIN << L"-" << EEV_VERSION_REV_WS;
    m_versionLabel.setText(versionString.str());
    m_versionLabel.setPrestyle(scene::Label::Prestyle::MENU_SOBER);
    m_versionLabel.setRelativePosition({1.f, 1.f});
    m_versionLabel.setRelativeOrigin({1.f, 1.f});

    // Functors
    auto singlePlayer = [this]() { stackPush(StateID::MENU_SELECTWORLD); };
    auto multiPlayer = nullptr;
    auto villains = [this]() { stackPush(StateID::MENU_MANAGEVILLAINS); };
    auto configuration = [this]() { stackPush(StateID::MENU_CONFIG); };
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
    m_reactImage.setShader("menu/name");
    m_reactImage.setMouseLeftDeselect(false);

    // Fading sprite
    nuiRoot.attachChild(m_fadingRectangle);
    m_fadingRectangle.setFillColor(sf::Color::Black);
    m_fadingRectangle.setDepth(-100.f);

    // Setting callbacks
    m_reactImage.loadFromFile("res/tex/menu/main/logo.xml");
    m_reactImage.setReactCallback(m_choices[0], singlePlayer);
    m_reactImage.setReactCallback(m_choices[1], multiPlayer);
    m_reactImage.setReactCallback(m_choices[2], villains);
    m_reactImage.setReactCallback(m_choices[3], configuration);
    m_reactImage.setReactCallback(m_choices[4], quitGame);
    m_reactImage.setActiveReact(L"V", false);
}

MenuMain::~MenuMain()
{
    // Musics
    Application::context().musics.stop("pippin_the_hunchback");

    // Freeing resources
    Application::freeTextures({"menu"});
}

//-------------------//
//----- Routine -----//

bool MenuMain::update(const sf::Time& dt)
{
    // The fading in black screen goes on
    if (m_fadingTime <= m_fadingDelay) {
        uint8 opacity = static_cast<uint8>(255.f * (1.f - m_fadingTime / m_fadingDelay));
        m_fadingRectangle.setFillColor({0u, 0u, 0u, opacity});
        m_fadingTime += dt;
    }

    // Checking if choiceBox changed
    if (m_choiceBox.choiceChanged()) {
        m_reactImage.setActiveReact(m_choices[m_choiceBox.selectedChoice()]);
    }

    // Checking if reactImage changed
    else if (m_reactImage.activeReactChanged()) {
        const auto* activeKey = m_reactImage.activeReactKey();
        if (activeKey != nullptr) {
            for (uint i = 0; i < m_choices.size(); ++i) {
                if (m_choices[i] == *activeKey) {
                    m_choiceBox.selectChoice(i);
                    break;
                }
            }
        }
    }

    return State::update(dt);
}

void MenuMain::handleEvent(const sf::Event& event)
{
    // Escape opens quit screen
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPush(StateID::QUIT);
        return;
    }

    State::handleEvent(event);
}

void MenuMain::refreshWindow(const config::WindowInfo& cWindow)
{
    const auto& resolution = cWindow.resolution;
    float maxSide = std::max(resolution.x, resolution.y);

    // Fading
    m_fadingRectangle.setSize(resolution);

    // Background
    auto textureSize = sf::v2f(Application::context().textures.get("menu/main/background").getSize());
    auto scaleFactor = maxSide / textureSize.x;
    m_background.setLocalScale({scaleFactor, scaleFactor});
    if (sf::Shader::isAvailable())
        Application::context().shaders.get("menu/background").setParameter("textureSize", scaleFactor * textureSize);

    baseClass::refreshWindow(cWindow);
}
