#pragma once

#include "resources/holder.hpp"
#include "resources/musicplayer.hpp"
#include "resources/soundplayer.hpp"
#include "resources/animationholder.hpp"
#include "resources/identifiers.hpp"
#include "states/statestack.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Text.hpp>

class Application
{
    struct Context {
        Context() {}
        ~Context() {}

        void init(const sf::Vector2f& iResolution, const std::string& iTitle, const uint32_t& iStyle);

        sf::Vector2f resolution;
        sf::Vector2f screenSize;
        sf::Vector2f effectiveDisplay;
        std::string title;
        uint32_t style;

        sf::RenderWindow window;
        TextureHolder textures;
        ShaderHolder shaders;
        FontHolder fonts;
        MusicPlayer music;
        SoundPlayer sounds;
        AnimationHolder animations;
    };

public:
    Application();
    virtual ~Application() {}

    void run();

    static inline Context& context() { return s_context; }

protected:
    void processInput();
    void update(const sf::Time& dt);
    void render();

    // Resources
    void loadTextures();
    void loadShaders();
    void loadSounds();
    void loadFonts();
    void loadAnimations();

    void refreshShaders();

    // States
    void registerStates();

    // Window management
    void refresh();
    sf::View bestView();
    void switchFullscreenMode();

private:
    static const sf::Time s_timePerFrame;
    static Context s_context;

    StateStack m_stateStack;
    States::ID m_initialState;

    float m_gameTime;
    bool m_running;
};
