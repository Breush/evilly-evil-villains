#include "dcb/bubble.hpp"

#include "context/context.hpp"
#include "tools/vector.hpp"

using namespace dcb;

Bubble::Bubble()
{
    setDetectable(false);

    // Background
    attachChild(m_frame);

    // Text
    m_frame.setContent(m_text);
    m_text.text().setFont(context::context.fonts.get("core/global/fonts/nui"));
    m_text.text().setFillColor(sf::Color::White);
}

//-------------------//
//----- Routine -----//

void Bubble::onChildSizeChanges(scene::Entity& child)
{
    returnif (&child != &m_frame);
    setSize(m_frame.size());
}

void Bubble::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_text.text().setCharacterSize(cNUI.fontSize);
}

//-------------------//
//----- Display -----//

void Bubble::fitWidth(float width)
{
    m_text.text().fitWidth(width);
}

//-----------------------------//
//----- Text manipulation -----//

void Bubble::forceMessage(const std::wstring& message)
{
    m_text.setString(message);
}

uint Bubble::addMessage(std::wstring message)
{
    m_messages.emplace_back(std::move(message));
    return m_messages.size() - 1u;
}

void Bubble::showMessage(uint messageID)
{
    massert(messageID < m_messages.size(), "Message ID " << messageID << " does not exists.");
    m_text.setString(m_messages[messageID]);
}

void Bubble::clearMessages()
{
    m_messages.clear();
}
