#include "dungeon/log.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "dungeon/data.hpp"
#include "config/nui.hpp"
#include "tools/tools.hpp"
#include "tools/platform-fixes.hpp" // reverse()

using namespace dungeon;

Log::Log()
{
    setDetectable(false);

    // Background
    m_background.setFillColor({255u, 255u, 255u, 192u});
    addPart(&m_background);

    refreshDisplay();
}

//-------------------//
//----- Routine -----//

void Log::onSizeChanges()
{
    refreshTextsWrap();
    refreshBackgroundSize();
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

void Log::refreshDisplay()
{
    config::NUI cNUI;

    m_characterSize = cNUI.fontSize;

    baseClass::refreshDisplay();
}

//------------------//
//----- Events -----//

void Log::receive(const Event& event)
{
    bool eventConsidered = true;
    std::wstringstream str;

    // Create text from event
    if (event.type == EventType::ROOM_CONSTRUCTED)
        str << L"CONSTRUCTED Room " << event.room.x << L"/" << event.room.y;
    else if (event.type == EventType::ROOM_DESTROYED)
        str << L"DESTROYED Room " << event.room.x << L"/" << event.room.y;
    else if (event.type == EventType::ERROR)
        str << L"[!] " << event.message;
    else
        eventConsidered = false;

    returnif (!eventConsidered);

    // Add message
    sf::Color color = (event.type == EventType::ERROR)? sf::Color::Red : sf::Color::Black;
    addMessage(std::move(str.str()), color);
}

//--------------------//
//----- Messages -----//

void Log::addMessage(std::wstring text, sf::Color color)
{
    Message message;

    message.text.setWrapString(std::move(text));
    message.text.setFont(Application::context().fonts.get(FontID::MONO));
    message.text.setCharacterSize(m_characterSize);
    message.text.setColor(std::move(color));
    message.text.fitWidth(size().x);

    m_currentHeight += boundsSize(message.text).y;

    m_messages.emplace_back(std::move(message));
    addPart(&m_messages.back().text);
    refreshMessages();
}

//------------------------------------//
//----- Internal changes updates -----//

void Log::refreshMessages()
{
    m_currentHeight = 0.f;
    for (auto& message : m_messages) {
        message.text.setPosition({0.f, m_currentHeight});
        m_currentHeight += boundsSize(message.text).y;
    }

    refreshBackgroundSize();
}

void Log::refreshTextsWrap()
{
    for (auto& message : m_messages)
        message.text.fitWidth(size().x);
}

void Log::refreshBackgroundSize()
{
    m_background.setSize({size().x, m_currentHeight});
}
