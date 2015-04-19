#include "dungeon/log.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "dungeon/data.hpp"
#include "tools/tools.hpp"
#include "tools/platform-fixes.hpp" // reverse()

using namespace dungeon;

Log::Log()
{
}

//-------------------//
//----- Routine -----//

void Log::update()
{
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
    else
        eventConsidered = false;

    returnif (!eventConsidered);

    // Create message
    Message message;
    message.text.setString(str.str());
    message.text.setFont(Application::context().fonts.get(FontID::MONO));
    message.text.setCharacterSize(16.f); // TODO From config
    message.text.setColor(sf::Color::White);

    // Add message
    m_messages.emplace_back(std::move(message));
    addPart(&m_messages.back().text);
    refreshMessages();
}

//------------------------------------//
//----- Internal changes updates -----//

void Log::refreshMessages()
{
    float y = 0.f;
    for (auto& message : m_messages) {
        message.text.setPosition({0.f, y});
        y += boundsSize(message.text).y;
    }
}
