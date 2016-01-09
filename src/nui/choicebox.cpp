#include "nui/choicebox.hpp"

#include "core/application.hpp"
#include "tools/tools.hpp"
#include "tools/vector.hpp"
#include "tools/debug.hpp"
#include "tools/string.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

using namespace nui;

ChoiceBox::ChoiceBox()
    : baseClass()
    , m_arrowsShowed(true)
{
    setFocusable(true);

    // Getting font from holder
    sf::Font& font = Application::context().fonts.get("nui");
    m_text.setFont(font);

    // Button
    m_buttonSprite.setTexture(Application::context().textures.get("nui/choicebox/button-idle"));

    // Add all parts
    addPart(&m_buttonSprite);
    addPart(&m_text);

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

    m_fontSize = cNUI.fontSize;
    m_arrowOffset = 2.f * cNUI.hPadding;
    m_arrowSize = cNUI.hintSize;

    // Update text
    m_text.setCharacterSize(m_fontSize);

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

//-----------------------------//
//----- Choice management -----//

void ChoiceBox::add(const std::initializer_list<std::wstring>& texts)
{
    for (const auto& text : texts)
        add(text);
}

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
        Application::context().sounds.play("refuse");
        return;
    }

    Application::context().sounds.play("accept");
    m_choices[m_selectedChoice].callback();
}

void ChoiceBox::switchChoiceLeft()
{
    if (m_choices.size() <= 1u) return;
    Application::context().sounds.play("select");

    if (m_selectedChoice == 0u) selectChoice(m_choices.size() - 1u);
    else selectChoice(m_selectedChoice - 1u);
}

void ChoiceBox::switchChoiceRight()
{
    if (m_choices.size() <= 1u) return;
    Application::context().sounds.play("select");

    if (m_selectedChoice == m_choices.size() - 1u) selectChoice(0u);
    else selectChoice(m_selectedChoice + 1u);
}

void ChoiceBox::selectChoice(const std::wstring& choiceText)
{
    for (uint i = 0u; i < m_choices.size(); ++i) {
        if (m_choices[i].text == choiceText) {
            m_selectedChoice = i;
            m_choiceChanged = true;
            refreshText();
            break;
        }
    }
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

void ChoiceBox::handleGlobalMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f&)
{
    returnif (!m_pressed);
    returnif (button != sf::Mouse::Left);

    m_pressed = false;
    m_buttonSprite.setTexture(Application::context().textures.get("nui/choicebox/button-idle"));
}

bool ChoiceBox::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    returnif (button != sf::Mouse::Left) false;

    m_pressed = true;
    m_buttonSprite.setTexture(Application::context().textures.get("nui/choicebox/button-pressed"));

    return true;
}

bool ChoiceBox::handleMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    returnif (!m_pressed) false;
    returnif (button != sf::Mouse::Left) false;

    // Without arrows: choices loop
    if (!m_arrowsShowed) {
        switchChoiceRight();
        return true;
    }

    // Left arrow selected
    if (isLeftArrowSelected(mousePos.x))
        switchChoiceLeft();
    // Right arrow selected
    else if (isRightArrowSelected(mousePos.x))
        switchChoiceRight();
    // With arrows, text selected
    else
        acceptChoice();

    return true;
}

bool ChoiceBox::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    resetPartsShader();

    // Hovering arrows
    if (isLeftArrowSelected(mousePos.x)) {
        setPartShader(&m_lArrow, "nui/hover");
        return true;
    }

    else if (isRightArrowSelected(mousePos.x)) {
        setPartShader(&m_rArrow, "nui/hover");
        return true;
    }

    // Text is being hovered
    returnif (m_selectedChoice == -1u || m_choices[m_selectedChoice].callback == nullptr) true;
    setPartShader(&m_text, "nui/hover");
    setPartShader(&m_buttonSprite, "nui/hover");
    return true;
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
    refreshArrows();
}

void ChoiceBox::refreshText()
{
    returnif (m_selectedChoice >= m_choices.size());

    m_text.setString(m_choices[m_selectedChoice].text);

    // If new choice need a callback, color if inactive
    if (m_choices[m_selectedChoice].callback == nullptr)
        m_text.setColor({150u, 150u, 150u, 255u});
    else
        m_text.setColor(sf::Color::White);

    // Position
    // Note: baseline of text is set to characterSize.y.
    auto textBounds = boundsSize(m_text);
    sf::Vector2f textOrigin(0.5f * textBounds.x, 0.75f * m_fontSize);
    m_text.setOrigin(textOrigin);
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
    auto buttonTextureSize = sf::v2f(m_buttonSprite.getTexture()->getSize());
    m_buttonSize = buttonTextureSize;

    for (auto& choice : m_choices) {
        auto text(m_text);
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

    // Arrows
    if (m_arrowsShowed)
        m_buttonSize.x += 2 * m_arrowOffset;

    // Reposition
    m_buttonSprite.setPosition({0.f, 0.f});
    m_buttonSprite.setScale(m_buttonSize / buttonTextureSize);
    m_text.setPosition(m_buttonSize.x * 0.5f, m_buttonSize.y * 0.5f);
}

void ChoiceBox::updateSize()
{
    updateButtonSize();
    setSize(m_buttonSize);
}

