#pragma once

#include "core/visualdebug.hpp"
#include "resources/holder.hpp"
#include "resources/musicplayer.hpp"
#include "resources/soundplayer.hpp"
#include "resources/animationholder.hpp"
#include "states/statestack.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>

class Application
{
    struct Context {
        Context() = default;
        ~Context() = default;

        void init(const sf::Vector2f& iResolution, const std::string& iTitle, const uint32_t& iStyle);

        sf::Vector2f resolution;
        sf::Vector2f screenSize;
        sf::Vector2f effectiveDisplay;
        std::string title;
        uint32_t style;

        sf::RenderWindow window;
        ViewHolder views;
        TextureHolder textures;
        ShaderHolder shaders;
        FontHolder fonts;
        MusicPlayer musics;
        SoundPlayer sounds;
        AnimationHolder animations;
    };

public:
    Application();
    virtual ~Application() = default;

    void run();

    static inline Context& context() { return s_context; }

protected:
    void processInput();
    void update(const sf::Time& dt);
    void render();

    // Resources
    void loadViews();
    void loadTextures();
    void loadShaders();
    void loadFonts();
    void loadMusics();
    void loadSounds();
    void loadAnimations();

    void refreshViews();
    void refreshShaders();

    void updateShaders(const sf::Time& dt);
    void updateAnimations(const sf::Time& dt);

    // States
    void loadStates();

    // Window management
    void clearWindowEvents();
    void clearWindowEvents(sf::Event& event, sf::Event::EventType type);
    void switchFullscreenMode();
    void refresh();

private:
    static const sf::Time s_timePerFrame;
    static Context s_context;

    StateStack m_stateStack;
    StateID m_initialState;

    float m_gameTime;
    bool m_running;

    VisualDebug m_visualDebug;
};
