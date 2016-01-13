#include "nui/dropdownlist.hpp"

#include "tools/tools.hpp"
#include "tools/string.hpp"

using namespace nui;

DropDownList::DropDownList()
    : baseClass()
    , m_selector(*this)
{
    setFocusable(true);

    // Selector
    m_selector.setDepth(-100.f);

    // Header
    addPart(&m_headerLeft);
    addPart(&m_headerMiddle);
    addPart(&m_headerRight);

    const auto& headerLeftTexture = Application::context().textures.get("nui/dropdownlist/header_left");
    const auto& headerMiddleTexture = Application::context().textures.get("nui/dropdownlist/header_middle");
    const auto& headerRightTexture = Application::context().textures.get("nui/dropdownlist/header_right");

    m_headerLeft.setTexture(&headerLeftTexture);
    m_headerMiddle.setTexture(&headerMiddleTexture);
    m_headerRight.setTexture(&headerRightTexture);

    m_leftWidth = static_cast<float>(headerLeftTexture.getSize().x);
    m_rightWidth = static_cast<float>(headerRightTexture.getSize().x);

    m_headerRight.setOrigin(m_rightWidth, 0.f);

    // Text
    addPart(&m_text);
    m_text.setFont(Application::context().fonts.get("nui"));
    m_text.setColor(sf::Color::White);
}

//-------------------//
//----- Routine -----//

void DropDownList::updateRoutine(const sf::Time& dt)
{
    // Remove flags
    m_justDropped = false;
}

void DropDownList::onSizeChanges()
{
    refresh();
}

void DropDownList::onTransformChanges()
{
    auto absolutePosition = localPosition() - getOrigin();
    m_selector.setLocalPosition({absolutePosition.x, absolutePosition.y + m_lineHeight});
}

void DropDownList::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    // As the selector is not always attached,
    // we force here the NUI refresh
    m_selector.refreshNUI(cNUI);

    // Decorum
    m_fontSize = cNUI.fontSize;
    m_hPadding = 1.5f * cNUI.hPadding;
    m_vPadding = cNUI.vPadding;

    // Text
    m_text.setCharacterSize(m_fontSize);

    // Lines
    m_lineHeight = m_fontSize + 2.f * m_vPadding;

    updateSize();
}

void DropDownList::updateSize()
{
    m_selector.updateSize();
    setSize({m_selector.size().x, m_lineHeight});
}

//------------------//
//----- Events -----//

void DropDownList::handleGlobalMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f&)
{
    returnif (button == sf::Mouse::Middle);
    returnif (m_justDropped);
    undropList();
}

bool DropDownList::handleKeyboardEvent(const sf::Event& event)
{
    returnif (event.type != sf::Event::KeyPressed) false;

    if (event.key.code == sf::Keyboard::Down) {
        undropList();
        selectPrevious();
        return true;
    }
    else if (event.key.code == sf::Keyboard::Up) {
        undropList();
        selectNext();
        return true;
    }

    return false;
}

bool DropDownList::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    returnif (button != sf::Mouse::Left) false;

    if (m_dropped)  undropList();
    else            dropList();

    return true;
}

bool DropDownList::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    resetPartsShader();

    setPartShader(&m_text, "nui/hover");
    setPartShader(&m_headerLeft, "nui/hover");
    setPartShader(&m_headerMiddle, "nui/hover");
    setPartShader(&m_headerRight, "nui/hover");

    return true;
}

void DropDownList::handleMouseLeft()
{
    resetPartsShader();
}

//-----------------------------//
//----- Choice management -----//

void DropDownList::add(const std::initializer_list<std::wstring>& texts)
{
    for (const auto& text : texts)
        add(text);
}

void DropDownList::add(std::wstring string)
{
    // Undrop the list to be sure there are no extra shown parts
    undropList();

    m_selector.add(std::move(string));
    updateSize();

    // Select first choice when added
    if (m_selector.choicesCount() == 1u)
        select(0u);
}

void DropDownList::clear()
{
    undropList();
    m_selector.clear();
    m_selectedChoiceID = -1u;
    updateSize();
}

void DropDownList::selectPrevious()
{
    auto choicesCount = m_selector.choicesCount();
    returnif (choicesCount <= 1u);

    Application::context().sounds.play("select");

    if (m_selectedChoiceID == 0u) select(choicesCount - 1u);
    else select(m_selectedChoiceID - 1u);
}

void DropDownList::selectNext()
{
    auto choicesCount = m_selector.choicesCount();
    returnif (choicesCount <= 1u);

    Application::context().sounds.play("select");

    if (m_selectedChoiceID == choicesCount - 1u) select(0u);
    else select(m_selectedChoiceID + 1u);
}

void DropDownList::select(const std::wstring& choiceString)
{
    auto choicesCount = m_selector.choicesCount();

    for (uint choiceID = 0u; choiceID < choicesCount; ++choiceID) {
        if (m_selector.choiceString(choiceID) == choiceString) {
            select(choiceID);
            break;
        }
    }
}

void DropDownList::select(uint choiceID)
{
    returnif (m_selectedChoiceID == choiceID);

    auto choicesCount = m_selector.choicesCount();
    massert(choiceID < choicesCount, "Choice " << choiceID << " out of range.");

    // Setting new choice
    m_selectedChoiceID = choiceID;
    m_text.setString(m_selector.choiceString(choiceID));
}

//--------------------//
//----- Dropping -----//

void DropDownList::dropList()
{
    returnif (m_dropped);
    returnif (parent() == nullptr);

    m_dropped = true;
    m_justDropped = true;
    parent()->attachChild(m_selector);
}

void DropDownList::undropList()
{
    returnif (!m_dropped);
    returnif (parent() == nullptr);

    m_dropped = false;
    parent()->detachChild(m_selector);
}

//---------------//
//----- ICU -----//

void DropDownList::refresh()
{
    // Text
    m_text.setPosition(m_hPadding, m_vPadding);

    // Header
    m_headerLeft.setPosition(0.f, 0.f);
    m_headerMiddle.setPosition(m_leftWidth, 0.f);
    m_headerRight.setPosition(size().x, 0.f);

    m_headerLeft.setSize({m_leftWidth, size().y});
    m_headerMiddle.setSize({size().x - (m_leftWidth + m_rightWidth), size().y});
    m_headerRight.setSize({m_rightWidth, size().y});
}
