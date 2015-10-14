#include "states/terminal.hpp"

#include "core/gettext.hpp"
#include "tools/platform-fixes.hpp" // make_unique()
#include "tools/tools.hpp"

using namespace states;

// TODO How to conserve historic?

Terminal::Terminal(StateStack& stack)
    : State(stack)
{
    // Creating scene
    auto& nuiRoot = nuiLayer().root();

    // Background
    nuiRoot.attachChild(m_background);
    m_background.setFillColor({255u, 255u, 255u, 192u});
    m_background.setRelativeOrigin({0.f, 1.f});

    // Command entry
    nuiRoot.attachChild(m_entry);
    m_entry.setRelativeOrigin({0.f, 1.f});
    m_entry.setRelativePosition({0.f, 1.f});
    m_entry.setOnValidateCallback([this] { onEntryValidated(); });
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
    return false;
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
}

void Terminal::handleEvent(const sf::Event& event)
{
    // Back to previous state on Escape
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPop();
        return;
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
    m_entry.setText(L"");

    addMessage(commandLine, sf::Color::Blue);
    auto commands = Application::context().commander.interpret(commandLine);
    Application::context().commander.push(commands);
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
    m_background.setLocalPosition({0.f, lastPosition});
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
    m_background.setSize({m_width, m_currentHeight});
}