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
    m_background.setOutlineThickness(1.f);
    m_background.setOutlineColor(sf::Color::White);
    m_background.setFillColor({255u, 255u, 255u, 10u});
    m_background.setPosition({1.f, 1.f});

    // Text
    addPart(&m_wrapText);
    m_wrapText.setFont(Application::context().fonts.get(FontID::MONO));
    m_wrapText.setColor(sf::Color::White);
    m_wrapText.setPosition({2.f, 2.f});

    refreshDisplay();
}

//-------------------//
//----- Routine -----//

void Bubble::onSizeChanges()
{
    m_background.setSize(size() - 2.f);

    m_wrapText.fitWidth(size().x - 4.f);
    setPartClippingRect(&m_wrapText, {0.f, 0.f, size().x - 4.f, size().y - 4.f});
}

void Bubble::refreshDisplay()
{
    config::NUI cNUI;

    m_wrapText.setCharacterSize(cNUI.fontSize);

    baseClass::refreshDisplay();
}

//-----------------------------//
//----- Text manipulation -----//

void Bubble::forceText(std::wstring text)
{
    m_wrapText.setWrapString(std::move(text));
}
