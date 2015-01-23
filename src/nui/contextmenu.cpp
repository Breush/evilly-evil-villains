#include "core/application.hpp"
#include "nui/contextmenu.hpp"
#include "nui/uicore.hpp"
#include "resources/holder.hpp"

using namespace nui;

ContextMenu::ContextMenu()
    : baseClass()
    , m_padding(5.f)
{
}

void ContextMenu::init()
{
    // Getting font from holder
    sf::Font& font = Application::context().fonts.get(Fonts::NUI);
    m_text.setCharacterSize(16);
    m_text.setFont(font);
    m_text.setColor(sf::Color::White);
    m_text.setPosition(m_padding, m_padding);

    // Background
    m_bg.setFillColor(sf::Color::Blue);
    m_bg.setOutlineColor(sf::Color::White);
    m_bg.setOutlineThickness(2);

    update();
}

void ContextMenu::update()
{
    // Reset
    clearParts();
    auto bounds = m_text.getLocalBounds();
    setSize({bounds.left + bounds.width + 2.f * m_padding, bounds.top + bounds.height + 2.f * m_padding});
    m_bg.setSize(size());

    // Adding parts
    addPart(&m_bg);
    addPart(&m_text);
}

void ContextMenu::changedMessage()
{
    m_text.setString(m_message);
    update();
}
