#include "dungeon/log.hpp"

#include "core/application.hpp"
#include "dungeon/data.hpp"
#include "dungeon/eventtype.hpp"
#include "config/nuiguides.hpp"
#include "tools/tools.hpp"
#include "tools/platform-fixes.hpp" // reverse()

using namespace dungeon;

Log::Log()
    : m_width(-1.f)
    , m_direction(Direction::NEW_BELOW)
{
    setDetectable(false);

    // Background
    m_background.setFillColor({255u, 255u, 255u, 192u});
    addPart(&m_background);

    // Entry
    attachChild(m_commandEntry);
    m_commandEntry.setRelativeOrigin({0.f, 1.f});
    m_commandEntry.setOnValidateCallback([this] { onCommandEntryValidated(); });
}

//-------------------//
//----- Routine -----//

void Log::onSizeChanges()
{
    // Background
    m_background.setSize({size().x, m_currentHeight});
}

void Log::updateRoutine(const sf::Time& dt)
{
    bool messagesChanged = false;

    for (auto it = m_messages.begin(); it != m_messages.end(); ) {
        auto& message = *it;

        // Increment alive time
        message.aliveSince += dt.asSeconds();

        // Remove messages that are too old
        if (message.aliveSince >= m_aliveTime) {
            removePart(&message.text);
            it = m_messages.erase(it);
            messagesChanged = true;
        }
        else ++it;
    }

    if (messagesChanged)
        refreshMessages();
}

void Log::refreshNUI(const config::NUIGuides& cNUI)
{
    m_characterSize = cNUI.fontSize;

    baseClass::refreshNUI(cNUI);
}

//------------------//
//----- Events -----//

void Log::receive(const context::Event& event)
{
    const auto& devent = *reinterpret_cast<const dungeon::Event*>(&event);
    bool eventConsidered = true;
    std::wstringstream str;

    // Create text from event
    if (devent.type == "room_constructed")
        str << L"CONSTRUCTED room " << devent.room.x << L"/" << devent.room.y;
    else if (devent.type == "room_destroyed")
        str << L"DESTROYED room " << devent.room.x << L"/" << devent.room.y;
    else if (devent.type == "room_exploded")
        str << L"Room EXPLODED " << devent.action.room.x << L"/" << devent.action.room.y;
    else if (devent.type == "error")
        str << L"[!] " << devent.message;
    else
        eventConsidered = false;

    returnif (!eventConsidered);

    // Add message
    sf::Color color = (devent.type == "error")? sf::Color::Red : sf::Color::Black;
    addMessage(std::move(str.str()), color);
}

//--------------------//
//----- Messages -----//

void Log::addMessage(std::wstring text, sf::Color color)
{
    Message message;

    message.text.setString(std::move(text));
    message.text.setFont(Application::context().fonts.get("mono"));
    message.text.setCharacterSize(m_characterSize);
    message.text.setColor(std::move(color));
    message.text.fitWidth(size().x);

    m_messages.emplace_back(std::move(message));
    addPart(&m_messages.back().text);
    refreshMessages();
}

//--------------------//
//----- Commands -----//

void Log::commandLog(const std::wstring& message)
{
    addMessage(message, {50u, 50u, 50u, 255u});
}

void Log::onCommandEntryValidated()
{
    auto commandLine = m_commandEntry.text();
    m_commandEntry.setText(L"");

    addMessage(commandLine, sf::Color::Blue);
    auto commands = Application::context().commander.interpret(commandLine);
    Application::context().commander.push(commands);
}

//------------------------------------//
//----- Internal changes updates -----//

void Log::refreshMessages()
{
    m_currentHeight = 0.f;

    // New messages appear below the stack
    if (m_direction == Direction::NEW_BELOW) {
        for (auto& message : m_messages) {
            message.text.setPosition({0.f, m_currentHeight});
            m_currentHeight += boundsSize(message.text).y;
        }
    }
    // Old messages appear below the stack
    else if (m_direction == Direction::OLD_BELOW) {
        for (auto& message : std::reverse(m_messages)) {
            message.text.setPosition({0.f, m_currentHeight});
            m_currentHeight += boundsSize(message.text).y;
        }
    }

    m_currentHeight += m_commandEntry.size().y + 5.f;
    refreshHeight();
}

void Log::refreshTextsWrap()
{
    returnif (m_width <= 0.f);
    for (auto& message : m_messages)
        message.text.fitWidth(m_width);
}

void Log::refreshSize()
{
    setSize({m_width, m_currentHeight});
    m_commandEntry.setWidth(m_width);
    refreshTextsWrap();
}

void Log::refreshHeight()
{
    m_commandEntry.setLocalPosition({0.f, m_currentHeight});
    setSize({m_width, m_currentHeight});
}
