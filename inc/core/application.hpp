#pragma once

#include "core/visualdebug.hpp"
#include "resources/holder.hpp"
#include "resources/musicplayer.hpp"
#include "resources/soundplayer.hpp"
#include "resources/animationholder.hpp"
#include "states/statestack.hpp"
#include "config/windowinfo.hpp"
#include "config/nuiguides.hpp"
#include "config/display.hpp"
#include "config/sound.hpp"
#include "tools/int.hpp"

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
        //! Constructor.
        Context() : windowInfo(display.window) {}

        //! Recreates the window from current config::WindowInfo.
        void recreateWindow();

        sf::RenderWindow window;    //!< The window, final destination of all draws.

        resources::TextureHolder    textures;   //!< The textures.
        resources::ShaderHolder     shaders;    //!< The shaders.
        resources::FontHolder       fonts;      //!< The fonts.
        resources::MusicPlayer      musics;     //!< The musics.
        resources::SoundPlayer      sounds;     //!< The sounds.
        resources::AnimationHolder  animations; //!< The animations, parsed SCML files.

        config::Display     display;    //!< The display configuration.
        config::Sound       sound;      //!< The sound configuration.
        config::NUIGuides   nuiGuides;  //!< Guidelines for NUI elements.
        config::WindowInfo  windowInfo; //!< Extra informations for window parameters.
    };

    //! Access the static context to get resources and window/screen information.
    static inline Context& context() { return s_context; }

public:

    //--------------------//
    //! @name Application
    //! @{

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

    //! @}

protected:

    //----------------------------//
    //! @name Logic and rendering
    //! @{

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

    //! @}

    //--------------------------//
    //! @name Window management
    //! @{

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

    //! @}

    //--------------------------------//
    //! @name Pre-loading and refresh
    //! @{

    void loadTextures();    //!< Load textures into memory.
    void loadShaders();     //!< Load shaders into memory.
    void loadFonts();       //!< Load fonts into memory.
    void loadMusics();      //!< Load musics file information.
    void loadSounds();      //!< Load sounds file information.
    void loadAnimations();  //!< Load animations into memory.
    void loadStates();      //!< Register states.

    void refreshNUI();      //!< Adapt all NUI elements.
    void refreshWindow();   //!< Adapt all views to current window settings.
    void refreshShaders();  //!< Adapt all shaders to current window settings.
    void refreshSounds();   //!< Adapt all sounds position/volume to current settings.

    void updateShaders(const sf::Time& dt);     //!< Animate the shaders.
    void updateAnimations(const sf::Time& dt);  //!< Animate the currently played animations.

    //! @}

private:

    //! The main global variable (and almost the only one).
    static Context s_context;

    //! Fixed timestep to update game logic.
    const sf::Time m_timePerFrame = sf::seconds(1.f/60.f);

    float m_gameTimeFactor = 1.f;   //!< Modifying the speed of the whole game.
    float m_gameTime = 0.f;         //!< The game time since game loop is running.
    bool m_running = false;         //!< Controls whether to continue game loop.
    StateID m_initialState;         //!< The initial state push into the stack (not fixed for easy debugging).

    // Visual part
    states::StateStack m_stateStack;    //!< The stack of states.
    VisualDebug m_visualDebug;          //!< The debug information.
};

