#include "states/menu/main.hpp"

#include "context/context.hpp"
#include "context/logger.hpp"
#include "core/gettext.hpp"
#include "core/define.hpp"
#include "tools/vector.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

using namespace states;

MenuMain::MenuMain(StateStack& stack)
    : baseClass(stack)
{
    // During menus, enable key repeat
    context::context.window.setKeyRepeatEnabled(true);
    context::context.musics.play("core/global/musics/pippin_the_hunchback");

    // Creating scene
    auto& nuiRoot = nuiLayer().root();

    // Background
    nuiRoot.attachChild(m_background);
    m_background.setDepth(100.f);
    m_background.setTexture("core/menu/main/background");

    // Copyright label
    nuiRoot.attachChild(m_copyrightLabel);
    m_copyrightLabel.setText(EEV_COPYRIGHT_SHORT_WS);
    m_copyrightLabel.setPrestyle(scene::Label::Prestyle::MENU_SOBER_LIGHT);
    m_copyrightLabel.setRelativePosition({0.f, 1.f});
    m_copyrightLabel.setRelativeOrigin({0.f, 1.f});

    // Version label
    nuiRoot.attachChild(m_versionLabel);
    std::wstringstream versionString;
    versionString << L"Evilly Evil Villains - " << EEV_VERSION_SWEET_NAME_WS << L" ";
    versionString << EEV_VERSION_MAJ << L"." << EEV_VERSION_MIN << L"-" << EEV_VERSION_REV_WS;
    m_versionLabel.setText(versionString.str());
    m_versionLabel.setPrestyle(scene::Label::Prestyle::MENU_SOBER);
    m_versionLabel.setRelativePosition({1.f, 1.f});
    m_versionLabel.setRelativeOrigin({1.f, 1.f});

    // Functors
    auto singlePlayer  = [this]() { stackPush(StateID::MENU_SELECTWORLD); };
    auto multiPlayer   = nullptr;
    auto villains      = [this]() { stackPush(StateID::MENU_MANAGEVILLAINS); };
    auto configuration = [this]() { stackPush(StateID::MENU_CONFIG); };
    auto quitGame      = [this]() { stackClear(); };

    // Stacker
    nuiRoot.attachChild(m_buttonsStacker);
    m_buttonsStacker.setRelativeOrigin({0.5f, 1.f});
    m_buttonsStacker.setRelativePosition({0.5f, 0.95f});

    // Choices
    m_choices.push_back({L"V", std::make_unique<nui::PushButton>()});
    m_choices.push_back({L"I", std::make_unique<nui::PushButton>()});
    m_choices.push_back({L"L", std::make_unique<nui::PushButton>()});
    m_choices.push_back({L"Y", std::make_unique<nui::PushButton>()});
    m_choices.push_back({L"S", std::make_unique<nui::PushButton>()});

    m_choices[0u].button->setValidateCallback(singlePlayer);
    m_choices[1u].button->setValidateCallback(multiPlayer);
    m_choices[2u].button->setValidateCallback(villains);
    m_choices[3u].button->setValidateCallback(configuration);
    m_choices[4u].button->setValidateCallback(quitGame);

    for (auto& choice : m_choices)
        m_buttonsStacker.stackBack(*choice.button);

    // Menu react image
    nuiRoot.attachChild(m_reactImage);
    m_reactImage.centerOrigin();
    m_reactImage.setRelativePosition({0.5f, 0.2f});
    m_reactImage.setShader("core/menu/main/logo");
    m_reactImage.setMouseLeftDeselect(false);

    // Fading sprite
    nuiRoot.attachChild(m_fadingRectangle);
    m_fadingRectangle.setFillColor(sf::Color::Black);
    m_fadingRectangle.setDepth(-100.f);

    // Setting callbacks
    m_reactImage.loadFromFile("res/core/menu/main/logo.xml");
    m_reactImage.setReactCallback(m_choices[0].key, singlePlayer);
    m_reactImage.setReactCallback(m_choices[1].key, multiPlayer);
    m_reactImage.setReactCallback(m_choices[2].key, villains);
    m_reactImage.setReactCallback(m_choices[3].key, configuration);
    m_reactImage.setReactCallback(m_choices[4].key, quitGame);
    m_reactImage.setActiveReact(L"V", false);
}

MenuMain::~MenuMain()
{
    // Musics
    context::context.musics.stop("core/global/musics/pippin_the_hunchback");
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

    // Translated strings
    m_choices[0u].button->setText(_("Solo"));
    m_choices[1u].button->setText(_("Multiplayer"));
    m_choices[2u].button->setText(_("Villains..."));
    m_choices[3u].button->setText(_("Options..."));
    m_choices[4u].button->setText(_("Quit"));

    // Fading
    m_fadingRectangle.setSize(resolution);

    // Background
    auto textureSize = sf::v2f(context::context.textures.get("core/menu/main/background").getSize());
    auto scaleFactor = maxSide / textureSize.x;
    m_background.setLocalScale({scaleFactor, scaleFactor});

    baseClass::refreshWindow(cWindow);
}

//-----------------------//
//----- Interpreter -----//

void MenuMain::interpret(std::vector<context::Command>& commands, std::vector<std::wstring>& tokens)
{
    std::wstring logMessage = L"> [menuMain] ";
    auto nTokens = tokens.size();

    if (nTokens == 1u) {
        if (tokens[0u] == L"exit") {
            logMessage += L"Exit";
            stackPop();
            goto logging;
        }
        else if (tokens[0u] == L"playSolo") {
            logMessage += L"> [menuMain] Single player";
            stackPush(StateID::MENU_SELECTWORLD);
            goto logging;
        }
    }

    return;

    logging:
    context::addCommandLog(commands, logMessage);
}

void MenuMain::autoComplete(std::vector<std::wstring>& possibilities, const std::vector<std::wstring>& tokens, const std::wstring& lastToken)
{
    auto nTokens = tokens.size();

    if (nTokens == 0u) {
        if (std::wstring(L"exit").find(lastToken) == 0u)        possibilities.emplace_back(L"exit");
        if (std::wstring(L"playSolo").find(lastToken) == 0u)    possibilities.emplace_back(L"playSolo");
    }
}
