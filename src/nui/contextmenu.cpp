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

    // Getting size
    sf::Vector2f border(2.f * padding(), 2.f * padding());
    for (auto& choice : m_choices) {
        auto bounds = choice.text.getLocalBounds();
        border.x = std::max(border.x, 2.f * padding() + bounds.left + bounds.width);
        border.y += choiceHeight();
    }

    setSize(border);
    m_bg.setSize(size());

    // Adding parts in the correct order
    addPart(&m_bg);
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
    uint choice = (fixPos.y - padding()) / choiceHeight();

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
    uint choice = (fixPos.y - padding()) / choiceHeight();

    resetPartsShader();
    if (choice < m_choices.size())
        setPartShader(&m_choices[choice].text, Shaders::NUI_HOVER);
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

void ContextMenu::addChoice(std::wstring text, Callback callback)
{
    ChoiceInfo choiceInfo;

    choiceInfo.text.setString(text);
    choiceInfo.callback = callback;

    // Getting font from holder
    sf::Font& font = Application::context().fonts.get(Fonts::NUI);
    choiceInfo.text.setCharacterSize(16);
    choiceInfo.text.setFont(font);
    choiceInfo.text.setColor(sf::Color::White);
    choiceInfo.text.setPosition(padding(), choiceHeight() * m_choices.size() + padding());

    m_choices.push_back(choiceInfo);
    update();
}
