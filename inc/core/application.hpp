#pragma once

#include "resources/holder.hpp"
#include "resources/musicplayer.hpp"
#include "resources/soundplayer.hpp"
#include "resources/identifiers.hpp"
#include "states/statestack.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Text.hpp>

class Application
{
    struct Context {
        Context(sf::Vector2f dimensions, const std::string& title, uint32_t style = sf::Style::Default)
            : resolution(dimensions)
            , window(sf::VideoMode(dimensions.x, dimensions.y), title, style)
        {}

        sf::Vector2f resolution;
        sf::RenderWindow window;
        TextureHolder textures;
        ShaderHolder shaders;
        FontHolder fonts;
        MusicPlayer music;
        SoundPlayer sounds;
    };

public:
    Application();
    void run();

    static inline Context& context() { return s_context; }

protected:
    void processInput();
    void update(sf::Time dt);
    void render();

    // Resources
    void loadTextures();
    void loadShaders();
    void loadFonts();

    // States
    void registerStates();

    // Window management
    sf::View bestView(const sf::Vector2f& windowSize);

private:
    static const sf::Time s_timePerFrame;
    static Context s_context;

    StateStack m_stateStack;

    States::ID m_initialState;
    float m_gameTime;
    bool m_running;
};
