#include "nui/choicebox.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "tools/tools.hpp"
#include "tools/vector.hpp"
#include "tools/debug.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

using namespace nui;

ChoiceBox::ChoiceBox()
    : baseClass()
    , m_arrowsShowed(true)
    , m_linesShowed(true)
{
    setFocusable(true);

    // Getting font from holder
    sf::Font& font = Application::context().fonts.get(FontID::NUI);
    m_text.setFont(font);

    // Add all parts
    addPart(&m_text);

    if (m_linesShowed) {
        addPart(&m_topLine);
        addPart(&m_botLine);
    }

    if (m_arrowsShowed) {
        addPart(&m_lArrow);
        addPart(&m_rArrow);
    }
}

//-------------------//
//----- Routine -----//

void ChoiceBox::updateRoutine(const sf::Time&)
{
    m_choiceChanged = false;
}

void ChoiceBox::onSizeChanges()
{
    refresh();
}

void ChoiceBox::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_fontVSpace = cNUI.fontVSpace;
    m_arrowOffset = 2.f * cNUI.hPadding;
    m_lineOffset = cNUI.hPadding;
    m_arrowSize = cNUI.hintSize;
    m_lineSize = cNUI.borderThick;

    // Update text
    m_text.setCharacterSize(cNUI.fontSize);

    updateSize();
}

//-------------------//
//----- Setters -----//

void ChoiceBox::showArrows(bool enabled)
{
    returnif (m_arrowsShowed == enabled);
    m_arrowsShowed = enabled;

    // Newly enabled
    if (m_arrowsShowed) {
        addPart(&m_lArrow);
        addPart(&m_rArrow);
        refresh();
    }
    // Newly disabled
    else {
        removePart(&m_lArrow);
        removePart(&m_rArrow);
    }
}

void ChoiceBox::showLines(bool enabled)
{
    returnif (m_linesShowed == enabled);
    m_linesShowed = enabled;

    // Newly enabled
    if (m_linesShowed) {
        addPart(&m_topLine);
        addPart(&m_botLine);
        refresh();
    }
    // Newly disabled
    else {
        removePart(&m_topLine);
        removePart(&m_botLine);
    }
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

void ChoiceBox::clearChoices()
{
    m_choices.clear();
    m_selectedChoice = -1u;
    m_choiceChanged = true;
    refresh();
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
    Application::context().sounds.play(SoundID::NUI_SELECT);

    if (m_selectedChoice == 0u) selectChoice(m_choices.size() - 1u);
    else selectChoice(m_selectedChoice - 1u);
}

void ChoiceBox::switchChoiceRight()
{
    if (m_choices.size() <= 1u) return;
    Application::context().sounds.play(SoundID::NUI_SELECT);

    if (m_selectedChoice == m_choices.size() - 1u) selectChoice(0u);
    else selectChoice(m_selectedChoice + 1u);
}

void ChoiceBox::selectChoice(uint choice)
{
    returnif (m_selectedChoice == choice);

    // Confirm valid choice
    massert(choice < m_choices.size(), "Choice " << choice << " out of range.");

    // Setting new choice
    m_selectedChoice = choice;
    m_choiceChanged = true;
    refreshText();
}

void ChoiceBox::setChoiceText(uint choice, const std::wstring& text)
{
    // Confirm valid choice
    massert(choice < m_choices.size(), "Choice " << choice << " out of range.");

    // Setting new choice
    m_choices[choice].text = text;

    // Max size or current text may have changed
    updateSize();
}

void ChoiceBox::setChoiceCallback(uint choice, Callback callback)
{
    // Confirm valid choice
    massert(choice < m_choices.size(), "Choice " << choice << " out of range.");

    // Setting new choice
    m_choices[choice].callback = callback;

    // Activeness of choice might have changed
    refreshText();
}

//------------------------//
//----- Mouse events -----//

void ChoiceBox::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    returnif (button != sf::Mouse::Left);

    // Without arrows: choices loop
    if (!m_arrowsShowed) {
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
    returnif (m_selectedChoice == -1u || m_choices[m_selectedChoice].callback == nullptr);
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
    returnif (!m_arrowsShowed) false;
    float arrowRange = m_arrowSize + m_arrowOffset / 2.f;
    return (x < arrowRange);
}

bool ChoiceBox::isRightArrowSelected(const float& x)
{
    returnif (!m_arrowsShowed) false;
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
    if (!m_arrowsShowed) {
        switchChoiceRight();
        return true;
    }

    // With arrows, check for callback
    acceptChoice();
    return true;
}

//-------------------//
//----- Updates -----//

void ChoiceBox::refresh()
{
    refreshText();
    refreshLines();
    refreshArrows();
}

void ChoiceBox::refreshText()
{
    returnif (m_selectedChoice >= m_choices.size());

    m_text.setString(m_choices[m_selectedChoice].text);

    // If new choice need a callback, color if inactive
    if (m_choices[m_selectedChoice].callback == nullptr)
        m_text.setColor(sf::Color(150, 150, 150));
    else
        m_text.setColor(sf::Color::White);

    // Position
    m_text.setOrigin(boundsSize(m_text) / 2.f);
}

void ChoiceBox::refreshLines()
{
    returnif (!m_linesShowed);

    m_topLine.setLength(m_buttonSize.x);
    m_botLine.setLength(m_buttonSize.x);
    m_topLine.setPosition(0.f, 0.f);
    m_botLine.setPosition(0.f, m_buttonSize.y - 1.f);
}

void ChoiceBox::refreshArrows()
{
    returnif (!m_arrowsShowed);

    m_lArrow.setLength(m_arrowSize);
    m_rArrow.setLength(m_arrowSize);
    m_lArrow.setPosition(0.f, 0.5f * m_buttonSize.y);
    m_rArrow.setPosition(m_buttonSize.x - m_arrowSize, 0.5f * m_buttonSize.y);
    m_lArrow.setOrigin(0.f, 0.5f * m_arrowSize);
    m_rArrow.setOrigin(0.f, 0.5f * m_arrowSize);
}

void ChoiceBox::updateButtonSize()
{
    bool emptyText = true;

    // Getting max size of all choices
    m_buttonSize = {0.f, m_fontVSpace};

    for (auto& choice : m_choices) {
        sf::Text text(m_text);
        text.setString(choice.text);
        const auto& bounds = text.getLocalBounds();
        m_buttonSize.x = std::max(m_buttonSize.x, (bounds.left + bounds.width));
        emptyText &= choice.text.empty();
    }

    // No text
    if (emptyText)
        m_buttonSize = {0.f, 0.f};

    // Max text size is the current button size, i.e. without decoration
    m_maxTextSize = m_buttonSize;

    // Lines
    if (m_linesShowed)
        m_buttonSize.y += 2 * (m_lineOffset + m_lineSize);

    // Arrows
    if (m_arrowsShowed)
        m_buttonSize.x += 2 * (m_arrowOffset + m_arrowSize);

    // Reposition text
    m_text.setPosition(m_buttonSize / 2.f);
}

void ChoiceBox::updateSize()
{
    updateButtonSize();
    setSize(m_buttonSize);
}

