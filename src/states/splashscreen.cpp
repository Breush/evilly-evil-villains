#include "states/splashscreen.hpp"

#include "context/context.hpp"
#include "context/logger.hpp"
#include "core/application.hpp"
#include "tools/vector.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>

using namespace states;

SplashScreen::SplashScreen(StateStack& stack)
    : State(stack)
{
    // Loading resources
    Application::loadTextures({"core/menu/splashscreen"});
    Application::loadAnimations({"core/menu/splashscreen"});

    // Creating scene
    auto& nuiRoot = nuiLayer().root();
    const auto& nuiSize = nuiLayer().size();
    float maxSide = std::max(nuiSize.x, nuiSize.y);

    // Background
    const auto& textureSize = context::context.textures.get("core/menu/splashscreen/background").getSize();
    nuiRoot.attachChild(m_background);
    m_background.setDepth(100.f);
    m_background.setTexture("core/menu/splashscreen/background");
    m_background.setShader("core/menu/splashscreen/background");
    m_background.setLocalScale(maxSide / sf::v2f(textureSize));
    m_background.setLocalPosition((nuiSize - maxSide) / 2.f);

    // Fading sprite
    nuiRoot.attachChild(m_fadingRectangle);
    m_fadingRectangle.setFillColor(sf::Color::Transparent);
    m_fadingRectangle.setDepth(-100.f);
    m_fadingRectangle.setSize(nuiSize);

    // Animation
    nuiRoot.attachChild(m_logo);
    m_logo.load("core/menu/splashscreen/logo");
    m_logo.setRelativePosition({0.5f, 0.5f});
    m_logo.restart();
}

SplashScreen::~SplashScreen()
{
    context::context.sounds.stopAll();

    // Freeing resources
    Application::freeTextures({"core/menu/splashscreen"});
    Application::freeAnimations({"core/menu/splashscreen"});
}

//-------------------//
//----- Routine -----//

bool SplashScreen::update(const sf::Time& dt)
{
    // Check on animated entities
    if (!m_logo.started()) {
        // If the animation is over, the fading out black screen goes on
        if (m_fadingTime >= m_fadingDelay) {
            stackReplace(StateID::MENU_MAIN);
            return false;
        }

        uint8 opacity = static_cast<uint8>(255.f * m_fadingTime / m_fadingDelay);
        m_fadingRectangle.setFillColor({0u, 0u, 0u, opacity});
        m_fadingTime += dt;
    }

    return State::update(dt);
}

void SplashScreen::handleEvent(const sf::Event& event)
{
    // Skip splashscreen on Escape
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        skip();
        return;
    }

    State::handleEvent(event);
}

//-----------------------//
//----- Interpreter -----//

context::CommandPtr SplashScreen::interpret(const std::vector<std::wstring>& tokens)
{
    std::wstring logMessage;
    auto nTokens = tokens.size();

    if (nTokens == 1u) {
        if (tokens[0u] == L"skip") {
            logMessage = L"> [splashScreen] Skipping";
            skip();
        }
    }

    if (logMessage.empty()) return nullptr;

    auto pCommand = std::make_unique<context::Command>();
    context::setCommandLog(*pCommand, logMessage);
    return std::move(pCommand);
}

void SplashScreen::autoComplete(std::vector<std::wstring>& possibilities, const std::vector<std::wstring>& tokens, const std::wstring& lastToken)
{
    auto nTokens = tokens.size();

    if (nTokens == 0u) {
        if (std::wstring(L"skip").find(lastToken) == 0u)    possibilities.emplace_back(L"skip");
    }
}

//-------------------//
//----- Actions -----//

void SplashScreen::skip()
{
    stackReplace(StateID::MENU_MAIN);
}
