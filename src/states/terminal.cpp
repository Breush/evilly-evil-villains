#include "states/terminal.hpp"

#include "core/gettext.hpp"
#include "tools/platform-fixes.hpp" // make_unique()
#include "tools/tools.hpp"

using namespace states;

//----------------------------//
//----- Static variables -----//

std::deque<std::wstring> Terminal::s_historic;

//--------------------//
//----- Terminal -----//

Terminal::Terminal(StateStack& stack)
    : State(stack)
{
    // Creating scene
    auto& nuiRoot = nuiLayer().root();

    // Background
    nuiRoot.attachChild(m_background);
    m_background.setFillColor({0u, 0u, 0u, 192u});
    m_background.setDepth(500.f);

    // Background for messages
    nuiRoot.attachChild(m_messagesBackground);
    m_messagesBackground.setFillColor({255u, 255u, 255u, 192u});
    m_messagesBackground.setRelativeOrigin({0.f, 1.f});

    // Command entry
    nuiRoot.attachChild(m_entry);
    m_entry.setRelativeOrigin({0.f, 1.f});
    m_entry.setRelativePosition({0.f, 1.f});
    m_entry.setOnValidateCallback([this] { onEntryValidated(); });

    // Pause/unpause button
    nuiRoot.attachChild(m_pauseButton);
    m_pauseButton.setRelativeOrigin({1.f, 0.f});
    m_pauseButton.setRelativePosition({0.99f, 0.02f});
    m_pauseButton.setText(L"Pause/Unpause");
    m_pauseButton.setFocusable(false);
    m_pauseButton.setCallback([this] { onPauseButtonPressed(); });

    // Initialize historic
    m_historicPos = 0u;
    if (s_historic.empty())
        s_historic.emplace_front();

    Application::setPaused(true);
}

Terminal::~Terminal()
{
    Application::setPaused(false);
}

//-------------------//
//----- Routine -----//

// FIXME These game pause don't stop animations or shaders...
bool Terminal::update(const sf::Time& dt)
{
    bool messagesChanged = false;

    for (auto it = m_messages.begin(); it != m_messages.end(); ) {
        auto& message = *it;

        // Increment alive time
        message.aliveSince += dt.asSeconds();

        // Remove messages that are too old
        if (message.aliveSince >= m_aliveTime) {
            nuiLayer().root().detachChild(*message.label);
            it = m_messages.erase(it);
            messagesChanged = true;
        }
        else ++it;
    }

    // Update size
    if (messagesChanged)
        refreshMessagesPositions();

    baseClass::update(dt);
    return !Application::paused();
}

void Terminal::refreshNUI(const config::NUIGuides& cNUI)
{
    m_characterSize = cNUI.fontSize;

    baseClass::refreshNUI(cNUI);
}

void Terminal::refreshWindow(const config::WindowInfo& cWindow)
{
    baseClass::refreshWindow(cWindow);

    const auto& resolution = cWindow.resolution;

    m_width = resolution.x / 2.f;
    m_entry.setWidth(resolution.x / 2.f);
    m_background.setSize(resolution);
}

void Terminal::handleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed) {
        // Back to previous state on Escape
        if (event.key.code == sf::Keyboard::Escape) {
            stackPop();
            return;
        }

        // Auto-complete
        if (event.key.code == sf::Keyboard::Tab) {
            auto commandLine = m_entry.textBeforeCursor();
            commandLine = Application::context().commander.autoComplete(std::move(commandLine));
            m_entry.setText(commandLine + m_entry.textAfterCursor(), false);
            m_entry.setCursorPosition(commandLine.size());
            return;
        }

        // Up/down for historic
        if (event.key.code == sf::Keyboard::Up) {
            navigateHistoric(1);
            return;
        }

        if (event.key.code == sf::Keyboard::Down) {
            navigateHistoric(-1);
            return;
        }
    }

    // Intercept some signals
    if (event.type == sf::Event::TextEntered) {
        returnif (event.text.unicode == 9); // TAB
    }

    State::handleEvent(event);
}

//--------------------//
//----- Messages -----//

void Terminal::addMessage(std::wstring text, sf::Color color)
{
    Message message;

    message.label = std::make_unique<scene::WrapLabel<sf::Text>>();
    message.label->setText(std::move(text));
    message.label->setFont("mono");
    message.label->setCharacterSize(m_characterSize);
    message.label->setColor(std::move(color));
    message.label->fitWidth(m_width);

    m_messages.emplace_back(std::move(message));
    nuiLayer().root().attachChild(*m_messages.back().label);
    refreshMessagesPositions();
}

//--------------------//
//----- Commands -----//

void Terminal::commandLog(const std::wstring& message)
{
    addMessage(message, {50u, 50u, 50u, 255u});
}

void Terminal::onEntryValidated()
{
    auto commandLine = m_entry.text();

    // Historic reset
    s_historic.front() = commandLine;
    s_historic.emplace_front();
    m_historicPos = 0u;

    returnif (commandLine.empty());
    m_entry.setText(L"");

    // Print command line
    addMessage(commandLine, sf::Color::Blue);

    // Interpret it via the commander
    auto commands = Application::context().commander.interpret(commandLine);
    Application::context().commander.push(commands);
}

void Terminal::navigateHistoric(const int relPos)
{
    // Remember current new, just in case
    if (m_historicPos == 0u)
        s_historic.front() = m_entry.text();

    returnif ((m_historicPos + relPos) >= s_historic.size());

    m_historicPos += relPos;
    m_entry.setText(s_historic.at(m_historicPos), false);
}

//-------------------//
//----- Control -----//

void Terminal::onPauseButtonPressed()
{
    Application::setPaused(!Application::paused());

    if (Application::paused()) m_background.setFillColor({0u, 0u, 0u, 192u});
    else m_background.setFillColor({0u, 0u, 0u, 126u});
}

//------------------------------------//
//----- Internal changes updates -----//

void Terminal::refreshMessagesPositions()
{
    float lastPosition = nuiLayer().root().size().y - m_entry.size().y;
    m_currentHeight = 0.f;

    // New messages appear below the stack
    for (auto& message : std::reverse(m_messages)) {
        message.label->setLocalPosition({0.f, lastPosition - m_currentHeight - message.label->size().y});
        m_currentHeight += message.label->size().y;
    }

    // Refreshing background
    m_messagesBackground.setLocalPosition({0.f, lastPosition});
    refreshSize();
}

void Terminal::refreshFromWidth()
{
    returnif (m_width <= 0.f);

    m_entry.setWidth(m_width);
    for (auto& message : m_messages)
        message.label->fitWidth(m_width);

    refreshSize();
}

void Terminal::refreshSize()
{
    m_messagesBackground.setSize({m_width, m_currentHeight});
}
