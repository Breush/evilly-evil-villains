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

    // Menu theme
    Application::context().musics.play("pippin_the_hunchback");

    // Creating scene
    auto& nuiRoot = nuiLayer().root();
    const auto& nuiSize = nuiLayer().size();
    float maxSide = std::max(nuiSize.x, nuiSize.y);

    // Background
    auto textureSize = sf::v2f(Application::context().textures.get("menu/main/background").getSize());
    auto scaleFactor = maxSide / textureSize.x;
    nuiRoot.attachChild(m_background);
    m_background.setDepth(100.f);
    m_background.setTexture("menu/main/background");
    m_background.setShader("menu/background");
    m_background.setLocalScale({scaleFactor, scaleFactor});

    if (sf::Shader::isAvailable())
        Application::context().shaders.get("menu/background").setParameter("textureSize", scaleFactor * textureSize);

    // Copyright label
    nuiRoot.attachChild(m_copyrightLabel);
    m_copyrightLabel.setText(EEV_COPYRIGHT_SHORT_WS);
    m_copyrightLabel.setPrestyle(scene::Label::Prestyle::MENU_SOBER_LIGHT);
    m_copyrightLabel.setLocalPosition({0.f, nuiSize.y - m_copyrightLabel.size().y});

    // Version label
    nuiRoot.attachChild(m_versionLabel);
    std::wstringstream versionString;
    versionString << EEV_VERSION_SWEET_NAME_WS << L" ";
    versionString << EEV_VERSION_MAJ << L"." << EEV_VERSION_MIN << L"-" << EEV_VERSION_REV_WS;
    m_versionLabel.setText(versionString.str());
    m_versionLabel.setPrestyle(scene::Label::Prestyle::MENU_SOBER);
    m_versionLabel.setLocalPosition(nuiSize - m_versionLabel.size());

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

    // Setting callbacks
    m_reactImage.loadFromFile("res/tex/menu/main/logo.xml");
    m_reactImage.setReactCallback(m_choices[0], singlePlayer);
    m_reactImage.setReactCallback(m_choices[1], multiPlayer);
    m_reactImage.setReactCallback(m_choices[2], villains);
    m_reactImage.setReactCallback(m_choices[3], configuration);
    m_reactImage.setReactCallback(m_choices[4], quitGame);
}

MenuMain::~MenuMain()
{
    // Freeing resources
    Application::freeTextures({"menu"});
}

//-------------------//
//----- Routine -----//

bool MenuMain::update(const sf::Time& dt)
{
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
