#include "dcb/bubble.hpp"

#include "config/nui.hpp"
#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "tools/vector.hpp"

using namespace dcb;

Bubble::Bubble()
{
    setDetectable(false);

    // Background
    addPart(&m_background);
    m_background.setOutlineColor(sf::Color::White);
    m_background.setFillColor({255u, 255u, 255u, 10u});

    // Text
    addPart(&m_wrapText);
    m_wrapText.setFont(Application::context().fonts.get(FontID::NUI));
    m_wrapText.setColor(sf::Color::White);

    refreshDisplay();
}

//-------------------//
//----- Routine -----//

void Bubble::onSizeChanges()
{
    refreshParts();
}

void Bubble::refreshDisplay()
{
    config::NUI cNUI;

    m_wrapText.setCharacterSize(cNUI.fontSize);
    m_hPadding = cNUI.hPadding;
    m_vPadding = cNUI.vPadding;

    refreshParts();
    baseClass::refreshDisplay();
}

//-----------------------------//
//----- Text manipulation -----//

void Bubble::forceText(std::wstring text)
{
    m_wrapText.setWrapString(std::move(text));
}

//-----------------------------------//
//----- Internal change updates -----//

void Bubble::refreshParts()
{
    m_background.setOutlineThickness(m_outlineThickness);
    m_background.setPosition({m_outlineThickness, m_outlineThickness});
    m_background.setSize(size() - 2.f * m_outlineThickness);

    m_wrapText.setPosition({m_hPadding, m_vPadding});
    m_wrapText.fitWidth(size().x - 2.f * m_hPadding);
    setPartClippingRect(&m_wrapText, {0.f, 0.f, size().x - 2.f * m_hPadding, size().y - 2.f * m_vPadding});
}
