#include "nui/choicebox.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "tools/tools.hpp"
#include "tools/debug.hpp"
#include "config/nui.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

using namespace nui;

ChoiceBox::ChoiceBox()
    : baseClass()
    , m_showArrows(true)
    , m_showLines(true)
{
    setFocusable(true);

    // Getting font from holder
    sf::Font& font = Application::context().fonts.get(FontID::NUI);
    m_text.setFont(font);

    refreshDisplay();
}

//-------------------//
//----- Routine -----//

void ChoiceBox::updateRoutine(const sf::Time&)
{
    m_choiceChanged = false;
}

void ChoiceBox::update()
{
    returnif (m_text.getString().isEmpty());

    clearParts();
    addPart(&m_text);

    const float lineSpace = m_lineOffset + m_lineSize;
    const float arrowSpace = m_arrowOffset + m_arrowSize;

    // Text (using integer position to prevent text smoothing)
    auto bounds = m_text.getLocalBounds();
    sf::Vector2f textSize(bounds.left + bounds.width, bounds.top + bounds.height);
    sf::Vector2u textPos((m_maxTextSize - textSize) / 2.f);
    m_text.setPosition(sf::Vector2f(textPos));

    // Lines
    if (showLines()) {
        addPart(&m_topLine);
        addPart(&m_botLine);

        m_text.move(0.f, lineSpace);
        m_topLine.setLength(m_buttonSize.x);
        m_botLine.setLength(m_buttonSize.x);
        m_topLine.setPosition(0.f, 0.f);
        m_botLine.setPosition(0.f, m_buttonSize.y);
    }

    // Arrows
    if (showArrows()) {
        addPart(&m_lArrow);
        addPart(&m_rArrow);

        m_text.setPosition(m_text.getPosition() + sf::Vector2f(arrowSpace, 0.f));
        m_lArrow.setLength(m_arrowSize);
        m_rArrow.setLength(m_arrowSize);
        m_lArrow.setPosition(0.f, 0.5f * m_buttonSize.y);
        m_rArrow.setPosition(m_buttonSize.x - m_arrowSize, 0.5f * m_buttonSize.y);
        m_lArrow.setOrigin(0.f, 0.5f * m_arrowSize);
        m_rArrow.setOrigin(0.f, 0.5f * m_arrowSize);
    }
}

void ChoiceBox::refreshDisplay()
{
    config::NUI cNUI;

    m_fontVSpace = cNUI.fontVSpace;
    m_arrowOffset = 2.f * cNUI.hPadding;
    m_lineOffset = cNUI.hPadding;
    m_arrowSize = cNUI.hintSize;
    m_lineSize = cNUI.borderThick;

    // Update text
    m_text.setCharacterSize(cNUI.fontSize);

    updateSize();
    baseClass::refreshDisplay();
}

//-----------------------------//
//----- Choice management -----//

void ChoiceBox::add(const std::wstring& text, const Callback callback)
{
    m_choices.push_back({text, callback});

    // Max size might have changed, need to recompute size
    updateSize();

    // Select first choice when added
    if (m_choices.size() == 1u)
        selectChoice(0u);
}

void ChoiceBox::acceptChoice()
{
    // Maybe callback is not set
    if (m_choices[m_selectedChoice].callback == nullptr) {
        Application::context().sounds.play(SoundID::NUI_REFUSE);
        return;
    }

    Application::context().sounds.play(SoundID::NUI_ACCEPT);
    m_choices[m_selectedChoice].callback();
}

void ChoiceBox::switchChoiceLeft()
{
    if (m_choices.size() <= 1u) return;
    m_selectedChoice = ((m_selectedChoice == 0)? m_choices.size() : m_selectedChoice) - 1u;
    Application::context().sounds.play(SoundID::NUI_SELECT);
    selectChoice(m_selectedChoice);
}

void ChoiceBox::switchChoiceRight()
{
    if (m_choices.size() <= 1u) return;
    if (++m_selectedChoice == m_choices.size()) m_selectedChoice = 0u;
    Application::context().sounds.play(SoundID::NUI_SELECT);
    selectChoice(m_selectedChoice);
}

void ChoiceBox::selectChoice(uint choice)
{
    // Confirm valid choice
    massert(choice < m_choices.size(), "Choice " << choice << " out of range.");

    // Setting new choice
    m_selectedChoice = choice;
    m_text.setString(m_choices[choice].text);
    m_choiceChanged = true;

    // If new choice need a callback, color if inactive
    if (showArrows() && m_choices[choice].callback == nullptr)
        m_text.setColor(sf::Color(150, 150, 150));
    else
        m_text.setColor(sf::Color::White);

    update();
}

void ChoiceBox::setChoiceText(uint choice, const std::wstring& text)
{
    // Confirm valid choice
    massert(choice < m_choices.size(), "Choice " << choice << " out of range.");

    // Setting new choice
    m_choices[choice].text = text;

    // Max size or current text may have changed
    updateSize();
    selectChoice(m_selectedChoice);
}

void ChoiceBox::setChoiceCallback(uint choice, Callback callback)
{
    // Confirm valid choice
    massert(choice < m_choices.size(), "Choice " << choice << " out of range.");

    // Setting new choice
    m_choices[choice].callback = callback;

    // Activeness of choice might have changed
    selectChoice(m_selectedChoice);
}

//------------------------//
//----- Mouse events -----//

void ChoiceBox::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    returnif (button != sf::Mouse::Left);

    // Without arrows: choices loop
    if (!showArrows()) {
        switchChoiceRight();
        return;
    }

    // With arrows and arrow selected
    if (isLeftArrowSelected(mousePos.x)) {
        switchChoiceLeft();
        return;
    }

    if (isRightArrowSelected(mousePos.x)) {
        switchChoiceRight();
        return;
    }

    // With arrows, text selected
    acceptChoice();
}

void ChoiceBox::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    resetPartsShader();

    // Hovering arrows
    if (isLeftArrowSelected(mousePos.x)) {
        setPartShader(&m_lArrow, ShaderID::NUI_HOVER);
        return;
    }

    if (isRightArrowSelected(mousePos.x)) {
        setPartShader(&m_rArrow, ShaderID::NUI_HOVER);
        return;
    }

    // Text is being hovered
    setPartShader(&m_text, ShaderID::NUI_HOVER);
}

void ChoiceBox::handleMouseLeft()
{
    // Resetting hovering
    resetPartsShader();
}

//--------------------//
//----- Hovering -----//

bool ChoiceBox::isLeftArrowSelected(const float& x)
{
    returnif (!showArrows()) false;
    float arrowRange = m_arrowSize + m_arrowOffset / 2.f;
    return (x < arrowRange);
}

bool ChoiceBox::isRightArrowSelected(const float& x)
{
    returnif (!showArrows()) false;
    float arrowRange = m_arrowSize + m_arrowOffset / 2.f;
    return (x > size().x - arrowRange);
}

//---------------------------//
//----- Keyboard events -----//

bool ChoiceBox::handleKeyboardEvent(const sf::Event& event)
{
    // Just manage pressed keys
    returnif (event.type != sf::Event::KeyPressed) false;

    // Left or right to select
    if (event.key.code == sf::Keyboard::Left) {
        switchChoiceLeft();
        return true;
    }
    else if (event.key.code == sf::Keyboard::Right) {
        switchChoiceRight();
        return true;
    }

    // Just pressing Return is interesting now
    returnif (event.key.code != sf::Keyboard::Return) false;

    // Without arrows: choices loop
    if (!showArrows()) {
        switchChoiceRight();
        return true;
    }

    // With arrows, check for callback
    acceptChoice();
    return true;
}

//-------------------//
//----- Updates -----//

void ChoiceBox::updateButtonSize()
{
    m_buttonSize = {0.f, m_fontVSpace};

    // Getting max size of all choices
    for (auto& choice : m_choices) {
        sf::Text text(m_text);
        text.setString(choice.text);
        const auto& bounds = text.getLocalBounds();
        m_buttonSize.x = std::max(m_buttonSize.x, (bounds.left + bounds.width));
    }
    m_maxTextSize = m_buttonSize;

    // Lines
    if (showLines())
        m_buttonSize.y += 2 * (m_lineOffset + m_lineSize);

    // Arrows
    if (showArrows())
        m_buttonSize.x += 2 * (m_arrowOffset + m_arrowSize);
}

void ChoiceBox::updateSize()
{
    updateButtonSize();
    setSize(m_buttonSize); // Calls update()
}

