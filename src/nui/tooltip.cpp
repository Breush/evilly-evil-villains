#include "nui/tooltip.hpp"

using namespace nui;

Tooltip::Tooltip()
{
    attachChild(m_background);
    m_background.setFillColor({0u, 0u, 0u, 222u});
    m_background.setOutlineColor(sf::Color::White);
    m_background.setOutlineThickness(1.f);
    m_background.setDepth(-999.f);

    m_background.attachChild(m_text);
    m_text.setFont("nui");
    m_text.setColor(sf::Color::White);
    m_text.setLocalPosition({5.f, 5.f});
}

//-------------------//
//----- Routine -----//

void Tooltip::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_text.setCharacterSize(cNUI.fontSize);

    refresh();
}

//-------------------//
//----- Control -----//

void Tooltip::setText(const std::wstring& text)
{
    m_text.setText(text);
    refresh();
}

//---------------//
//----- ICU -----//

void Tooltip::refresh()
{
    auto backgroundSize = m_text.size() + 10.f;
    m_background.setSize(backgroundSize);
    m_background.setOrigin({0.5f * backgroundSize.x, backgroundSize.y + 5.f});
}
