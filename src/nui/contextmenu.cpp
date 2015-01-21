#include "core/application.hpp"
#include "nui/contextmenu.hpp"
#include "nui/uicore.hpp"
#include "resources/holder.hpp"

using namespace nui;

ContextMenu::ContextMenu()
    : baseClass()
{
}

void ContextMenu::init()
{
    // Getting font from holder
    sf::Font& font = Application::context().fonts.get(Fonts::NUI);
    m_text.setCharacterSize(16);
    m_text.setFont(font);
    m_text.setString("Hello");
    m_text.setColor(sf::Color::White);

    auto bounds = m_text.getLocalBounds();
    setSize({bounds.left + bounds.width, bounds.top + bounds.height});

    update();
}

void ContextMenu::update()
{
    clearParts();
    addPart(&m_text);
}
