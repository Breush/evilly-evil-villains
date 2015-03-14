#pragma once

#include "core/visualdebug.hpp"
#include "resources/holder.hpp"
#include "resources/musicplayer.hpp"
#include "resources/soundplayer.hpp"
#include "resources/animationholder.hpp"
#include "states/statestack.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/System/NonCopyable.hpp>
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

class Application final : sf::NonCopyable
{
public:

    //-------------------//
    //----- Context -----//

    //! This structure holds all resources managers and window/screen information.
    struct Context
    {
        //! Initialize window and screen context.
        /*!
         *  @param iResolution  The requested resolution.
         *  @param iTitle       The title of the window.
         *  @param iStyle       The SFML flags to set the style of the window.
         */
        void init(const sf::Vector2f& iResolution, const std::string& iTitle, const uint32_t& iStyle);

        sf::Vector2f resolution;        //!< The resolution asked.
        sf::FloatRect viewport;         //!< The current viewport.
        sf::Vector2f screenSize;        //!< Represents the dimension of the drawable part of the window,
                                        //! it should be reset at each resize.
        sf::Vector2f effectiveDisplay;  //!< Represents the dimensions of the viewport centered
                                        //! for the window, keeping the same ratio as resolution.
        std::string title;              //!< The title of the window.
        uint32_t style;                 //!< The style of the window, as defined by SFML.

        sf::RenderWindow window;                //!< The window, final destination of all draws.
        resources::TextureHolder textures;      //!< The textures.
        resources::ShaderHolder shaders;        //!< The shaders.
        resources::FontHolder fonts;            //!< The fonts.
        resources::MusicPlayer musics;          //!< The musics.
        resources::SoundPlayer sounds;          //!< The sounds.
        resources::AnimationHolder animations;  //!< The animations, parsed SCML files.
    };

    //! Access the static context to get resources and window/screen information.
    static inline Context& context() { return s_context; }

public:

    //-----------------------//
    //----- Application -----//

    //! Application constructor.
    Application();

    //! The game loop.
    /*!
     *  - Get the elapsed time since last frame
     *  - Process the inputs
     *  - Update the game logic at a fixed rate (m_timePerFrame)
     *  - Quit if there is no state in the stack
     *  - Then, finally, render the current scene
     */
    void run();

protected:

    //-------------------------------//
    //----- Logic and rendering -----//

    //! Input polling.
    /*!
     *  Handles primary events such as window closing, or resize.
     *  Also treat special keys (F3 for debug screen, F11 for fullscreen).
     *  And finally dispatch all other events to the state stack.
     */
    void processInput();

    //! Update the game logic.
    /*!
     *  Dispatch the time (normally fixed) to main components:
     *  visual debug, shaders, animations and finally state stack.
     */
    void update(const sf::Time& dt);

    //! Render the current scene.
    /*!
     *  - Clear the entire screen
     *  - Draw the state stack
     *  - Draw the debug information
     *  - Display the window
     */
    void render();

    //-----------------------------//
    //----- Window management -----//

    //! Poll out all events from the window.
    void clearWindowEvents();

    //! Poll out all events and get the last one of a certain type.
    /*!
     *  @param event    Will be set to the last event in the queue of specified type.
     *                  If there is none, it remains unchanged.
     *  @param type     The specified type.
     */
    void clearWindowEvents(sf::Event& event, sf::Event::EventType type);

    //! Quick way switch to switch fullscreen mode, recreate a context and refresh.
    void switchFullscreenMode();

    //! Tells the components that need screen information to refresh.
    /*!
     *  Views and shaders will update there internal information.
     */
    void refresh();

    //-----------------------------------//
    //----- Pre-loading and refresh -----//

    void loadTextures();    //!< Load textures into memory.
    void loadShaders();     //!< Load shaders into memory.
    void loadFonts();       //!< Load fonts into memory.
    void loadMusics();      //!< Load musics file information.
    void loadSounds();      //!< Load sounds file information.
    void loadAnimations();  //!< Load animations into memory.
    void loadStates();      //!< Register states.

    void refreshDisplay();  //!< Adapt all views to current window settings.
    void refreshShaders();  //!< Adapt all shaders to current window settings.

    void updateShaders(const sf::Time& dt);     //!< Animate the shaders.
    void updateAnimations(const sf::Time& dt);  //!< Animate the currently played animations.

private:

    //! The main global variable (and almost the only one).
    static Context s_context;

    //! Fixed timestep to update game logic.
    const sf::Time m_timePerFrame = sf::seconds(1.f/60.f);

    //! The game time since game loop is running.
    float m_gameTime = 0.f;

    //! Controls whether to continue game loop.
    bool m_running = false;

    //! The initial state push into the stack (not fixed for easy debugging).
    StateID m_initialState;

    //! The stack of states.
    states::StateStack m_stateStack;

    //! The debug information.
    VisualDebug m_visualDebug;
};

