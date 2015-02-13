#include "nui/choicebox.hpp"

#include "core/application.hpp"
#include "tools/tools.hpp"
#include "tools/debug.hpp"
#include "resources/soundplayer.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

using namespace nui;

ChoiceBox::ChoiceBox()
    : baseClass()
    , m_showArrows(true)
    , m_showLines(true)
    , m_arrowOffset(8.f)
    , m_lineOffset(5.f)
    , m_arrowSize(10.f)
    , m_lineSize(1.f)
    , m_nChoices(0)
    , m_choice(0)
    , m_choiceChanged(true)
{
    setFocusable(true);

    // Getting font from holder
    sf::Font& font = Application::context().fonts.get(Fonts::NUI);
    m_text.setCharacterSize(16);
    m_text.setFont(font);

    update();
}

void ChoiceBox::updateRoutine(const sf::Time& dt)
{
    m_choiceChanged = false;
}

void ChoiceBox::update()
{
    returnif (m_text.getString().isEmpty());

    clearParts();
    addPart(&m_text);

    const float lineSpace = lineOffset() + lineSize();
    const float arrowSpace = arrowOffset() + arrowSize();

    // Text (using integer position to prevent text smoothing)
    sf::Vector2u textPos((m_maxTextSize - m_textSize)/2.f);
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
        m_lArrow.setLength(arrowSize());
        m_rArrow.setLength(arrowSize());
        m_lArrow.setPosition(0.f, 0.5f * m_buttonSize.y);
        m_rArrow.setPosition(m_buttonSize.x - arrowSize(), 0.5f * m_buttonSize.y);
        m_lArrow.setOrigin(0.f, 0.5f * arrowSize());
        m_rArrow.setOrigin(0.f, 0.5f * arrowSize());
    }
}

//-----------------------------//
//----- Choice management -----//

void ChoiceBox::add(const std::wstring& text, const Callback callback)
{
    m_choices.push_back(ChoiceInfo {text, callback});

    // Max size might have changed, need to recompute size
    updateSize();

    // Select first choice when added
    if (!m_nChoices++)
        setChoice(0);
}

void ChoiceBox::acceptChoice()
{
    // Maybe callback is not set
    if (m_choices[m_choice].callback == nullptr) {
        Application::context().sounds.play(Sounds::NUI_REFUSE);
        return;
    }

    Application::context().sounds.play(Sounds::NUI_ACCEPT);
    m_choices[m_choice].callback();
}

void ChoiceBox::switchChoiceLeft()
{
    if (m_nChoices <= 1) return;
    m_choice = ((m_choice == 0)? m_nChoices : m_choice) - 1;
    Application::context().sounds.play(Sounds::NUI_SELECT);
    setChoice(m_choice);
}

void ChoiceBox::switchChoiceRight()
{
    if (m_nChoices <= 1) return;
    if (++m_choice == m_nChoices) m_choice = 0;
    Application::context().sounds.play(Sounds::NUI_SELECT);
    setChoice(m_choice);
}

void ChoiceBox::setChoice(uint choice)
{
    // Confirm valid choice
    massert(choice < m_nChoices, "Choice " << choice << " out of range");

    // Setting new choice
    m_choice = choice;
    m_text.setString(m_choices[choice].text);
    m_choiceChanged = true;

    // If new choice need a callback, color if inactive
    if (showArrows() && m_choices[choice].callback == nullptr)
        m_text.setColor(sf::Color(150, 150, 150));
    else
        m_text.setColor(sf::Color::White);

    // New text might need to be recentered
    auto bounds = m_text.getLocalBounds();
    m_textSize = {bounds.left + bounds.width, bounds.top + bounds.height};

    update();
}

void ChoiceBox::setChoiceText(uint choice, const std::wstring& text)
{
    // Confirm valid choice
    massert(choice < m_nChoices, "Choice " << choice << " out of range");

    // Setting new choice
    m_choices[choice].text = text;

    // Max size or current text may have changed
    updateSize();
    setChoice(m_choice);
}

void ChoiceBox::setChoiceCallback(uint choice, Callback callback)
{
    // Confirm valid choice
    massert(choice < m_nChoices, "Choice " << choice << " out of range");

    // Setting new choice
    m_choices[choice].callback = callback;

    // Activeness of choice might have changed
    setChoice(m_choice);
}

//------------------------//
//----- Mouse events -----//

void ChoiceBox::handleMouseButtonPressed(const sf::Mouse::Button& button, const sf::Vector2f& mousePos)
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

void ChoiceBox::handleMouseMoved(const sf::Vector2f& mousePos)
{
    resetPartsShader();

    // Hovering arrows
    if (isLeftArrowSelected(mousePos.x)) {
        setPartShader(&m_lArrow, Shaders::NUI_HOVER);
        return;
    }

    if (isRightArrowSelected(mousePos.x)) {
        setPartShader(&m_rArrow, Shaders::NUI_HOVER);
        return;
    }

    // Text is being hovered
    setPartShader(&m_text, Shaders::NUI_HOVER);
}

void ChoiceBox::handleMouseLeft()
{
    // Resetting hovering
    resetPartsShader();
}

bool ChoiceBox::isLeftArrowSelected(const float& x)
{
    returnif (!showArrows()) false;
    float arrowRange = arrowSize() + arrowOffset() / 2.f;
    return (x < arrowRange);
}

bool ChoiceBox::isRightArrowSelected(const float& x)
{
    returnif (!showArrows()) false;
    float arrowRange = arrowSize() + arrowOffset() / 2.f;
    return (x > size().x - arrowRange);
}

//---------------------------//
//----- Keyboard events -----//

void ChoiceBox::handleKeyboardEvent(const sf::Event& event)
{
    // Just manage pressed keys
    returnif (event.type != sf::Event::KeyPressed);

    // Left or right to select
    if (event.key.code == sf::Keyboard::Left) {
        switchChoiceLeft();
        return;
    }
    else if (event.key.code == sf::Keyboard::Right) {
        switchChoiceRight();
        return;
    }

    // Just pressing Return is interesting now
    returnif (event.key.code != sf::Keyboard::Return);

    // Without arrows: choices loop
    if (!showArrows()) {
        switchChoiceRight();
        return;
    }

    // With arrows, check for callback
    acceptChoice();
}

//-------------------//
//----- Updates -----//

void ChoiceBox::updateButtonSize()
{
    m_buttonSize = {0.f, 0.f};

    // Getting max size of all choices
    for (auto& choice : m_choices) {
        sf::Text text(m_text);
        text.setString(choice.text);
        const auto& bounds = text.getLocalBounds();
        m_buttonSize.x = std::max(m_buttonSize.x, (bounds.left + bounds.width));
        m_buttonSize.y = std::max(m_buttonSize.y, (bounds.top + bounds.height));
    }
    m_maxTextSize = m_buttonSize;

    // Lines
    if (showLines())
        m_buttonSize.y += 2 * (lineOffset() + lineSize());

    // Arrows
    if (showArrows())
        m_buttonSize.x += 2 * (arrowOffset() + arrowSize());
}

void ChoiceBox::updateSize()
{
    updateButtonSize();
    setSize(buttonSize());
    update();
}

