#include "core/application.hpp"

#include "core/debug.hpp"
#include "core/gettext.hpp"
#include "context/context.hpp"
#include "context/componenter.hpp"
#include "states/identifiers.hpp"
#include "tools/vector.hpp"
#include "tools/string.hpp"
#include "tools/filesystem.hpp"
#include "tools/tools.hpp"

#include <SFML/Window/Event.hpp>
#include <string>

//----------------------------//
//----- Static variables -----//

VisualDebug Application::s_visualDebug;
bool Application::s_needRefresh = false;
bool Application::s_paused = false;

//-----------------//
//----- Extra -----//

//! Cleans files that are created and can interfere in debug mode.
void cleanExtraFiles()
{
    // Cleaning log files
    auto logFiles = listFiles("log/");
    std::sort(std::begin(logFiles), std::end(logFiles), [] (const FileInfo& f1, const FileInfo& f2) { return f1.name.compare(f2.name) >= 0; });
    uint commandsCount = 0u, steamCount = 0u, errorCount = 0u;
    for (const auto& fileInfo : logFiles) {
        bool toBeRemoved = false;
        if (fileInfo.name.find("commands_") == 0u)      toBeRemoved = ++commandsCount > 5u;
        else if (fileInfo.name.find("steam_") == 0u)    toBeRemoved = ++steamCount > 5u;
        else if (fileInfo.name.find("error_") == 0u)    toBeRemoved = ++errorCount > 5u;
        if (toBeRemoved)
            std::remove(fileInfo.fullName.c_str());
    }

#if DEBUG_GLOBAL > 0
    std::remove("saves/villains_saved.xml");
    std::remove("saves/worlds_saved.xml");

    for (const auto& fileInfo : listFiles("saves/", true))
        if (fileInfo.name == "dungeon_saved.xml")
            std::remove(fileInfo.fullName.c_str());
#endif
}

//-----------------------//
//----- Application -----//

Application::Application(const std::vector<std::string>& args)
    : m_initialState(StateID::SPLASHSCREEN)
{
    // Arguments
    if (args.size() != 0u) {
        m_scriptStream.open(args[0u]);
        scriptNextLine();
        mdebug_core_1("Reading file " + args[0u] + " as script.");
    }

    // Context and config
    context::context.windowInfo.title = "Evilly Evil Villains";
    i18n::initLanguagesList();
    refreshFromConfig();

    // Load all on start (except textures)
    preloadTextures();
    loadShaders();
    loadFonts();
    loadMusics();
    preloadSounds();
    preloadAnimations();
    loadStates();
    loadComponents();

    // All is ready, go for it
    m_stateStack.pushState(m_initialState);
    s_visualDebug.init();
    m_cursor.init();

    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
        mdebug_core_1("Cleaning saved files from last session. Disable this by pressing LControl during application start.");
        cleanExtraFiles();
    }
}

Application::~Application()
{
    freeComponents();

    if (m_scriptStream.is_open())
        m_scriptStream.close();
}

void Application::run()
{
    sf::Clock clock;
    sf::Clock debugClock;
    sf::Time lag(sf::Time::Zero);

    m_running = true;
    while (m_running) {
        // Getting time
        lag += clock.restart();

        // Inputs
        processInput();
        synchronizeMouse();

        // Update until frame limit is hit
        while (lag >= m_updateTime) {
            lag -= m_updateTime;

            // Game logic core
            debugClock.restart();
            update(m_updateTime);
            s_visualDebug.setLogicTickTime(debugClock.getElapsedTime());

            // Quit if no more states
            if (m_stateStack.isEmpty())
                m_running = false;
        }

        // Rendering
        // We might be doing render(lag/m_updateTime);
        // So that physics can interpolate the effective display
        // See http://gameprogrammingpatterns.com/game-loop.html
        //  However, this seems a bit too much,
        // and SFML would not let us do that easily.
        debugClock.restart();
        render();
        s_visualDebug.setRenderTickTime(debugClock.getElapsedTime());
    }

    // Finish closing
    if (context::context.window.isOpen())
        context::context.window.close();
}

void Application::setPaused(bool paused)
{
    returnif (s_paused == paused);

    s_paused = paused;
    context::context.sounds.setPaused(s_paused);
    context::context.musics.setPaused(s_paused);
}

//-------------------------------//
//----- Logic and rendering -----//

void Application::processInput()
{
    sf::Event event;

    while (context::context.window.pollEvent(event)) {
        // Discard mouse moved events as we create them each frame anyway
        if (event.type == sf::Event::MouseMoved)
            continue;

        // Keyboard
        if (event.type == sf::Event::KeyPressed) {
            // Force refresh
            if (event.key.code == sf::Keyboard::F5) {
                s_needRefresh = true;
                continue;
            }

            // Switch visual debug mode
            if (event.key.code == sf::Keyboard::F3) {
                s_visualDebug.switchVisible();
                continue;
            }

            // Switch fullscreen mode
            if (event.key.code == sf::Keyboard::F11) {
                switchFullscreenMode();
                clearWindowEvents();
                break;
            }

            // Show terminal
            if (event.key.shift && event.key.code == sf::Keyboard::F2) {
                if (!m_stateStack.isStateVisible(StateID::TERMINAL))
                    m_stateStack.pushState(StateID::TERMINAL);
                continue;
            }

#if DEBUG_GLOBAL > 0
            // Hard reset on debug mode
            if (event.key.code == sf::Keyboard::BackSlash) {
                m_gameTime = 0.f;
                m_stateStack.clearStates();
                m_stateStack.pushState(StateID::SPLASHSCREEN);
                continue;
            }
#endif
        }

        // Closing window
        if (event.type == sf::Event::Closed) {
            if (!m_stateStack.isStateVisible(StateID::QUIT))
                m_stateStack.pushState(StateID::QUIT);
            clearWindowEvents();
            break;
        }

        // Resizing window (grab only the last of these events)
        if (event.type == sf::Event::Resized) {
            clearWindowEvents(event, sf::Event::Resized);

            // Security over the new size, a 0 dimension implies OpenGL bugs
            sf::Vector2u newSize(event.size.width, event.size.height);
            if (newSize.x < 10u || newSize.y < 10u) {
                    if (newSize.x < 10u) newSize.x = 10u;
                    if (newSize.y < 10u) newSize.y = 10u;
                    context::context.window.setSize(newSize);
                    continue;
            }

            context::context.windowInfo.screenSize = sf::v2f(newSize);
            refreshWindow();
            continue;
        }

        m_stateStack.handleEvent(event);
        m_cursor.handleEvent(event);
    }

}

void Application::synchronizeMouse()
{
    const auto& mousePosition = sf::Mouse::getPosition(context::context.window);

    sf::Event mouseMovedEvent;
    mouseMovedEvent.type = sf::Event::MouseMoved;
    mouseMovedEvent.mouseMove.x = mousePosition.x;
    mouseMovedEvent.mouseMove.y = mousePosition.y;
    m_stateStack.handleEvent(mouseMovedEvent);
    m_cursor.handleEvent(mouseMovedEvent);
}

void Application::update(const sf::Time& dt)
{
    // Refresh asked
    if (s_needRefresh) {
        refreshNUI();
        refreshWindow();
        s_needRefresh = false;
    }

    // Shaders can be animated
    m_gameTime += dt.asSeconds();

    // Overall elements
    s_visualDebug.update(dt);
    m_cursor.update(dt);

    // Command from script file?
    if (!m_scriptCommandLine.empty()) {
        if (m_scriptWaitTime > 0) {
            m_scriptWaitTime -= dt.asMilliseconds();
            m_scriptWaitTime = std::max(0, m_scriptWaitTime);
        }
        else {
            auto commands = context::context.commander.interpret(m_scriptCommandLine);
            context::context.commander.push(commands);
            if (!commands.empty()) {
                wdebug_core_2(L"Script command: " << m_scriptCommandLine);
                scriptNextLine();
            }
        }
    }

    // Game logic
    context::context.commander.update(dt);
    m_stateStack.update(dt);

    // Update the visual effects
    // FIXME Sounds and shaders are not affected by States's timefactor
    if (!s_paused) {
        updateSounds(dt);
        updateShaders(dt);
        updateComponents(dt);
    }
}

void Application::render()
{
    context::context.window.clear();
    context::context.window.draw(m_stateStack);
    context::context.window.draw(s_visualDebug);
    context::context.window.draw(m_cursor);
    context::context.window.display();
}

//-----------------------------//
//----- Window management -----//

void Application::refreshFromConfig(bool refreshWindow, bool refreshAudio)
{
    // Language
    i18n::init(toString(context::context.display.global.language));

    // Window
    if (refreshWindow) {
        context::context.windowInfo.style = (context::context.display.window.fullscreen)? sf::Style::Fullscreen : sf::Style::Default;
        context::context.recreateWindow();
    }

    // Audio
    if (refreshAudio) {
        refreshSounds();
        refreshMusics();
    }

    // Marking the need for future refresh of NUI/Window
    if (!refreshAudio || refreshWindow)
        s_needRefresh = true;
}

void Application::refreshNUI()
{
    context::context.nuiGuides.recompute(context::context.display.nui);

    m_stateStack.refreshNUI(context::context.nuiGuides);
    s_visualDebug.refreshNUI(context::context.nuiGuides);
}

void Application::refreshWindow()
{
    context::context.windowInfo.recompute();

    // Refresh all views
    m_stateStack.refreshWindow(context::context.windowInfo);
    s_visualDebug.refreshWindow(context::context.windowInfo);
    m_cursor.refreshWindow(context::context.windowInfo);

    // Refresh shaders
    refreshShaders();
}

void Application::clearWindowEvents()
{
    sf::Event polledEvent;
    while (context::context.window.pollEvent(polledEvent));
}

void Application::clearWindowEvents(sf::Event& event, sf::Event::EventType type)
{
    sf::Event polledEvent;
    while (context::context.window.pollEvent(polledEvent))
        if (polledEvent.type == type)
            event = polledEvent;
}

void Application::switchFullscreenMode()
{
    // Switching fullscreen flag
    context::context.display.window.fullscreen = !context::context.display.window.fullscreen;
    refreshFromConfig();
    refreshWindow();
}

//---------------------//
//----- Scripting -----//

void Application::scriptNextLine()
{
    bool theEnd;

    // This loop allows us to skip comments and blank lines
    bool empty = false;
    bool comment = false;
    bool commentBlock = false;
    do {
        theEnd = !std::getline(m_scriptStream, m_scriptCommandLine);
        empty = m_scriptCommandLine.empty();
        if (!theEnd && !empty) {
            comment = (m_scriptCommandLine[0u] == L'#');
            if (!comment) continue;
            if (commentBlock)   commentBlock = !(m_scriptCommandLine.size() >= 3u && m_scriptCommandLine[1u] == L'<' && m_scriptCommandLine[2u] == L'<');
            else                commentBlock =  (m_scriptCommandLine.size() >= 3u && m_scriptCommandLine[1u] == L'>' && m_scriptCommandLine[2u] == L'>');
        }
    } while (!theEnd && (empty || comment || commentBlock));

    if (theEnd) {
        m_scriptCommandLine.clear();
        return;
    }

    // Wait command is managed here
    returnif (m_scriptCommandLine.find(L"wait") != 0u);

    std::wstringstream ss(m_scriptCommandLine);
    ss >> m_scriptCommandLine;

    int waitTime = 0;
    ss >> waitTime;
    m_scriptWaitTime += waitTime;

    scriptNextLine();
}
