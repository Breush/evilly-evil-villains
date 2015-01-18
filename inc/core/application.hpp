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
public:
    Application();
    void run();

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

private:
    static const sf::Time TimePerFrame;

    sf::RenderWindow m_window;
    TextureHolder m_textures;
    FontHolder m_fonts;
    ShaderHolder m_shaders;
    StateStack m_stateStack;
    MusicPlayer	m_music;
    SoundPlayer	m_sounds;

    States::ID m_initialState;
    float m_gameTime;
    bool m_running;
};
