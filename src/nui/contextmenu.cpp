#include "core/application.hpp"
#include "nui/contextmenu.hpp"
#include "nui/uicore.hpp"
#include "resources/holder.hpp"

using namespace nui;

ContextMenu::ContextMenu()
    : baseClass()
    , m_padding(5)
    , m_choiceHeight(22)
{
    setFocusable(false);
}

void ContextMenu::init()
{
    // Background
    m_bg.setFillColor(sf::Color::Blue);
    m_bg.setOutlineColor(sf::Color::White);
    m_bg.setOutlineThickness(1); // TODO Have variable

    update();
}

void ContextMenu::update()
{
    // Reset
    clearParts();

    // Getting size and setting positions
    auto xOffset = padding();
    auto yOffset = padding();

    sf::Vector2f border(2.f * padding(), 2.f * padding());

    // Title
    if (!m_title.getString().isEmpty()) {
        auto bounds = m_title.getLocalBounds();
        m_title.setPosition(xOffset, yOffset);
        border.x = std::max(border.x, 2.f * padding() + bounds.left + bounds.width);
        border.y += choiceHeight();
        yOffset += choiceHeight();
    }

    // Choices
    for (auto& choice : m_choices) {
        auto bounds = choice.text.getLocalBounds();
        choice.text.setPosition(xOffset, yOffset);
        border.x = std::max(border.x, 2.f * padding() + bounds.left + bounds.width);
        border.y += choiceHeight();
        yOffset += choiceHeight();
    }

    setSize(border);
    m_bg.setSize(size());

    // Adding parts in the correct order
    addPart(&m_bg);
    if (!m_title.getString().isEmpty())
        addPart(&m_title);
    for (auto& choice : m_choices)
        addPart(&choice.text);

    setStatus(true);
}

//------------------------//
//----- Mouse events -----//

void ContextMenu::handleMouseEvent(const sf::Event& event, const sf::Vector2f& relPos)
{
    switch (event.type) {
    case sf::Event::MouseButtonPressed:
        handleMousePressed(event, relPos);
        break;
    case sf::Event::MouseMoved:
        handleMouseMoved(event, relPos);
        break;
    case sf::Event::MouseLeft:
        handleMouseLeft();
        break;
    default:
        break;
    }
}

void ContextMenu::handleMousePressed(const sf::Event&, const sf::Vector2f& relPos)
{
    sf::Vector2f fixPos = getInverseTransform().transformPoint(relPos);
    uint choice = choiceFromCoords(fixPos);

    if (choice < m_choices.size() && m_choices[choice].callback != nullptr) {
        Application::context().sounds.play(Sounds::NUI_ACCEPT);
        m_choices[choice].callback();
        setVisible(false);
    } else {
        Application::context().sounds.play(Sounds::NUI_REFUSE);
    }
}

void ContextMenu::handleMouseMoved(const sf::Event&, const sf::Vector2f& relPos)
{
    sf::Vector2f fixPos = getInverseTransform().transformPoint(relPos);
    uint choice = choiceFromCoords(fixPos);

    resetPartsShader();
    if (choice < m_choices.size())
        setPartShader(&m_choices[choice].text, Shaders::NUI_HOVER);
}

uint ContextMenu::choiceFromCoords(const sf::Vector2f& coords) const
{
    if (m_title.getString().isEmpty())
        return (coords.y - padding()) / choiceHeight();
    else
        return (coords.y - padding()) / choiceHeight() - 1;
}

void ContextMenu::handleMouseLeft()
{
    resetPartsShader();
}

//-------------------//
//----- Choices -----//

void ContextMenu::clearChoices()
{
    m_choices.clear();
}

void ContextMenu::setTitle(const std::wstring& title)
{
    sf::Font& font = Application::context().fonts.get(Fonts::NUI);
    m_title.setFont(font);
    m_title.setCharacterSize(16);
    m_title.setStyle(sf::Text::Bold);
    m_title.setColor(sf::Color::White);
    m_title.setString(title);

    update();
}

void ContextMenu::addChoice(const std::wstring& text, Callback callback)
{
    ChoiceInfo choiceInfo;

    choiceInfo.text.setString(text);
    choiceInfo.callback = callback;

    // Getting font from holder
    sf::Font& font = Application::context().fonts.get(Fonts::NUI);
    choiceInfo.text.setFont(font);
    choiceInfo.text.setCharacterSize(16);
    choiceInfo.text.setColor(sf::Color::White);

    m_choices.push_back(choiceInfo);
    update();
}
