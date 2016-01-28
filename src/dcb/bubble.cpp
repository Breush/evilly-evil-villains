#include "dcb/bubble.hpp"

#include "context/context.hpp"
#include "tools/vector.hpp"

using namespace dcb;

Bubble::Bubble()
{
    setDetectable(false);

    // Background
    addPart(&m_background);
    m_background.setOutlineColor(sf::Color::White);
    m_background.setFillColor({0u, 0u, 0u, 150u});

    // Text
    attachChild(m_text);
    m_text.text().setFont(context::context.fonts.get("core/global/fonts/nui"));
    m_text.text().setFillColor(sf::Color::White);
}

//-------------------//
//----- Routine -----//

void Bubble::onSizeChanges()
{
    refreshParts();
}

void Bubble::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_text.text().setCharacterSize(cNUI.fontSize);
    m_hPadding = cNUI.hPadding;
    m_vPadding = cNUI.vPadding;

    refreshParts();
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

//-----------------------------------//
//----- Internal change updates -----//

void Bubble::refreshParts()
{
    m_background.setOutlineThickness(m_outlineThickness);
    m_background.setPosition({m_outlineThickness, m_outlineThickness});
    m_background.setSize(size() - 2.f * m_outlineThickness);

    m_text.setLocalPosition({m_hPadding, m_vPadding});
    m_text.text().fitWidth(size().x - 2.f * m_hPadding);
}
