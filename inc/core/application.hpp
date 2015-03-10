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

//! The heart of the application.
/*! 
 *  Here is how things go:
 *  - Create the window context
 *  - Load every ressources
 *  - Push the initial state
 *  - Starts the game loop with a call to Application::run()
 */

class Application
{
public:

    //-------------------//
    //----- Context -----//

    //! This structure holds all resources managers and window/screen information.
    struct Context
    {
        //! Initialize window and screen context.
        /*!
         *  @param iResolution The requested resolution.
         *  @param iTitle The title of the window.
         *  @param iStyle The SFML flags to set the style of the window.
         */
        void init(const sf::Vector2f& iResolution, const std::string& iTitle, const uint32_t& iStyle);

        sf::Vector2f resolution;        //!< The resolution asked.
        sf::Vector2f screenSize;        //!< Represents the dimension of the drawable part of the window,
                                        //! it should be reset at each resize.
        sf::Vector2f effectiveDisplay;  //!< Represents the dimensions of the viewport centered
                                        //! for the window, keeping the same ratio as resolution.
        std::string title;              //!< The title of the window.
        uint32_t style;                 //!< The style of the window, as defined by SFML.

        sf::RenderWindow window;        //!< The window, final destination of all draws.
        ViewHolder views;               //!< The views, to separate user interface from scenary.
        TextureHolder textures;         //!< The textures.
        ShaderHolder shaders;           //!< The shaders.
        FontHolder fonts;               //!< The fonts.
        MusicPlayer musics;             //!< The musics.
        SoundPlayer sounds;             //!< The sounds.
        AnimationHolder animations;     //!< The animations, parsed SCML files.
    };

    //! Access the static context to get resources and window/screen information.
    static inline Context& context() { return s_context; }

public:

    Application();
    virtual ~Application() = default;

    void run();

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
    //! The main global variable (and almost the only one).
    static Context s_context;
    
    StateStack m_stateStack;
    StateID m_initialState;

    sf::Time m_timePerFrame = sf::seconds(1.f/60.f);
    float m_gameTime = 0.f;
    bool m_running = false;

    VisualDebug m_visualDebug;
};
