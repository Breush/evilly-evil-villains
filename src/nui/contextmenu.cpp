#include "nui/contextmenu.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "config/nui.hpp"

using namespace nui;

ContextMenu::ContextMenu()
    : baseClass()
{
    setVisible(false);

    // Background
    m_background.setTexture(&Application::context().textures.get(TextureID::DUNGEON_PANEL_BACKGROUND));

    // Title
    m_title.setFont(Application::context().fonts.get(FontID::NUI));
    m_title.setStyle(sf::Text::Bold);
    m_title.setColor(sf::Color::White);

    refreshDisplay();
}

//-------------------//
//----- Routine -----//

void ContextMenu::onSizeChanges()
{
    returnif (m_title.getString().isEmpty());

    // Reset
    clearParts();

    // Background
    addPart(&m_background);
    m_background.setSize(size());

    // Title
    if (!m_title.getString().isEmpty())
        addPart(&m_title);

    // Choices
    for (auto& choice : m_choices)
        addPart(&choice.text);
}

void ContextMenu::updateSize()
{
    // Getting size and setting positions
    auto xOffset = m_padding;
    auto yOffset = m_padding;

    sf::Vector2f border(2.f * m_padding, 2.f * m_padding);

    // Title
    if (!m_title.getString().isEmpty()) {
        auto bounds = m_title.getLocalBounds();
        m_title.setPosition(xOffset, yOffset);
        border.x = std::max(border.x, 2.f * m_padding + bounds.left + bounds.width);
        border.y += m_choiceHeight;
        yOffset += m_choiceHeight;
    }

    // Choices
    for (auto& choice : m_choices) {
        auto bounds = choice.text.getLocalBounds();
        choice.text.setPosition(xOffset, yOffset);
        border.x = std::max(border.x, 2.f * m_padding + bounds.left + bounds.width);
        border.y += m_choiceHeight;
        yOffset += m_choiceHeight;
    }

    setSize(border);
}

void ContextMenu::refreshDisplay()
{
    config::NUI cNUI;

    m_fontSize = cNUI.fontSize;
    m_padding = cNUI.hPadding;
    m_choiceHeight = m_padding + cNUI.fontVSpace;

    // Update font size
    m_title.setCharacterSize(m_fontSize);
    for (auto& choiceInfo : m_choices)
        choiceInfo.text.setCharacterSize(m_fontSize);

    updateSize();
    baseClass::refreshDisplay();
}

//------------------------//
//----- Mouse events -----//

void ContextMenu::handleGlobalEvent(const sf::Event& event)
{
    // Hide on click anywhere
    if (event.type == sf::Event::MouseButtonPressed && visible())
        markForVisible(false);
}

void ContextMenu::handleMouseButtonPressed(const sf::Mouse::Button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    uint choice = choiceFromCoords(mousePos);

    if (choice < m_choices.size() && m_choices[choice].callback != nullptr) {
        Application::context().sounds.play(SoundID::NUI_ACCEPT);
        m_choices[choice].callback();
    } else {
        Application::context().sounds.play(SoundID::NUI_REFUSE);
        markForVisible(true);
    }
}

void ContextMenu::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    uint choice = choiceFromCoords(mousePos);

    resetPartsShader();
    if (choice < m_choices.size() && m_choices[choice].callback != nullptr)
        setPartShader(&m_choices[choice].text, ShaderID::NUI_HOVER);
}

void ContextMenu::handleMouseLeft()
{
    resetPartsShader();
}

//-------------------//
//----- Choices -----//

uint ContextMenu::choiceFromCoords(const sf::Vector2f& coords) const
{
    if (m_title.getString().isEmpty())
        return (coords.y - m_padding) / m_choiceHeight;
    else
        return (coords.y - m_padding) / m_choiceHeight - 1;
}

void ContextMenu::clearChoices()
{
    m_choices.clear();
}

void ContextMenu::setTitle(const std::wstring& title)
{
    m_title.setString(title);
    updateSize();
}

void ContextMenu::addChoice(const std::wstring& text, Callback callback)
{
    ChoiceInfo choiceInfo;

    choiceInfo.text.setString(text);
    choiceInfo.callback = callback;

    // Getting font from holder
    sf::Font& font = Application::context().fonts.get(FontID::NUI);
    choiceInfo.text.setFont(font);
    choiceInfo.text.setCharacterSize(m_fontSize);

    if (callback == nullptr) choiceInfo.text.setColor(sf::Color(150, 150, 150));
    else choiceInfo.text.setColor(sf::Color::White);

    m_choices.emplace_back(std::move(choiceInfo));
    updateSize();
}
