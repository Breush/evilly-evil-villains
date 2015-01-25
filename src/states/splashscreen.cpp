#include "states/splashscreen.hpp"

#include "core/application.hpp"
#include "tools/tools.hpp"
#include "resources/holder.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <iostream>

SplashScreenState::SplashScreenState(StateStack& stack)
    : State(stack)
    //, m_data("res/scml/hero/hero.scml")
{
    const sf::Vector2f& windowSize = Application::context().resolution;

    // Background
    sf::Texture& texture = Application::context().textures.get(Textures::MENU_BG);
    m_bgSprite.setTexture(texture);
    m_bgSprite.setPosition((windowSize - sf::Vector2f(texture.getSize())) / 2.f);

    // Shader
    m_bgShader = &Application::context().shaders.get(Shaders::MENU_BG);

    /*// TODO Animation Handler
    // Loading animation
    m_fs.load(&m_data);
    printf("Loaded %zu images.\n", m_fs.images.size());

    for(const auto& entityInfo : m_data.entities) {
        SCML::Entity* entity = new SCML::Entity(&m_data, entityInfo.first);
        entity->setFileSystem(&m_fs);
        entity->setScreen(&Application::context().window);
        entity->getAnimation(0)->looping = "true";
        m_entities.push_back(entity);
    }
    printf("Loaded %zu entities.\n", m_entities.size());*/
}

SplashScreenState::~SplashScreenState()
{
    m_entities.clear();
    m_data.clear();
}

void SplashScreenState::draw()
{
    auto& window = Application::context().window;
    const auto& resolution = Application::context().resolution;
//    const auto& window = Application::context().window;

    // Animated background
    window.draw(m_bgSprite, m_bgShader);

    // TODO Animation handler
    // Draw animated entities
    /*for(const auto& entity : m_entities)
        entity->draw(resolution.x / 2.f, resolution.y / 2.f, 0.f, 1.f);*/
}

bool SplashScreenState::update(sf::Time dt)
{
    // TODO Animation handler
    // Update animated entities
    /*for(const auto& entity : m_entities)
        entity->update(dt.asMilliseconds());*/

    return true;
}

bool SplashScreenState::handleEvent(const sf::Event& event)
{
    // If any key is pressed, trigger the next screen
    if(event.type == sf::Event::KeyPressed) {
        stackPopPush(States::MENU_MAIN);
    }

    return true;
}
