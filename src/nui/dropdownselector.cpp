#include "nui/dropdownselector.hpp"

#include "nui/dropdownlist.hpp"
#include "tools/tools.hpp"
#include "tools/string.hpp"

using namespace nui;

DropDownSelector::DropDownSelector(DropDownList& dropDownList)
    : baseClass()
    , m_dropDownList(dropDownList)
{
    // Content
    attachChild(m_scrollArea);
    m_scrollArea.setContent(m_tableLayout);

    // Fill
    addPart(&m_fillLeft);
    addPart(&m_fillMiddle);
    addPart(&m_fillRight);

    const auto& fillLeftTexture = Application::context().textures.get("nui/dropdownlist/fill_left");
    const auto& fillMiddleTexture = Application::context().textures.get("nui/dropdownlist/fill_middle");
    const auto& fillRightTexture = Application::context().textures.get("nui/dropdownlist/fill_right");

    m_fillLeft.setTexture(&fillLeftTexture);
    m_fillMiddle.setTexture(&fillMiddleTexture);
    m_fillRight.setTexture(&fillRightTexture);

    m_leftWidth = fillLeftTexture.getSize().x;
    m_rightWidth = fillRightTexture.getSize().x;

    m_fillRight.setOrigin(m_rightWidth, 0.f);

    // Bottom
    addPart(&m_bottomLeft);
    addPart(&m_bottomMiddle);
    addPart(&m_bottomRight);

    const auto& bottomLeftTexture = Application::context().textures.get("nui/dropdownlist/bottom_left");
    const auto& bottomMiddleTexture = Application::context().textures.get("nui/dropdownlist/bottom_middle");
    const auto& bottomRightTexture = Application::context().textures.get("nui/dropdownlist/bottom_right");

    m_bottomLeft.setTexture(&bottomLeftTexture);
    m_bottomMiddle.setTexture(&bottomMiddleTexture);
    m_bottomRight.setTexture(&bottomRightTexture);

    m_bottomHeight = bottomMiddleTexture.getSize().y;

    m_bottomLeft.setOrigin(0.f, m_bottomHeight);
    m_bottomMiddle.setOrigin(0.f, m_bottomHeight);
    m_bottomRight.setOrigin(m_rightWidth, m_bottomHeight);

    // Highlights
    addPart(&m_hoverHighlight);
    m_hoverHighlight.setFillColor(sf::Color::Transparent);
}

//-------------------//
//----- Routine -----//

void DropDownSelector::onSizeChanges()
{
    refresh();
}

void DropDownSelector::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_hPadding = cNUI.hPadding;
    m_vPadding = cNUI.vPadding;

    // Table
    m_lineHeight = cNUI.fontSize + 2.f * cNUI.vPadding;
    m_tableLayout.setDimensions(m_choices.size(), 1u, m_lineHeight);

    for (uint i = 0u; i < m_choices.size(); ++i)
        m_tableLayout.setRowAdapt(i, Adapt::FIXED, m_lineHeight);
}

void DropDownSelector::updateSize()
{
    // Max width of all choices
    float maxTextBoundsWidth = 0.f;
    for (const auto& choice : m_choices)
        if (choice.label->size().x > maxTextBoundsWidth)
            maxTextBoundsWidth = choice.label->size().x;

    // All choices list height
    float listHeight = m_choices.size() * m_lineHeight;
    float smallListHeight = std::min(3u * m_lineHeight, listHeight);

    // Compute the new size
    sf::Vector2f newSize;
    newSize.x = maxTextBoundsWidth + m_hPadding + m_rightWidth;
    newSize.x = std::max(6.f * m_hPadding + m_rightWidth, newSize.x);
    newSize.y = smallListHeight + 0.5f * m_vPadding;
    setSize(newSize);

    // Content
    m_tableLayout.setSize({0.f, listHeight});
    m_scrollArea.setSize({newSize.x, smallListHeight});
}

//------------------//
//----- Events -----//

bool DropDownSelector::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left) false;

    float yOffset = mousePos.y - m_tableLayout.localPosition().y;
    uint choiceID = static_cast<uint>(yOffset / m_lineHeight);
    if (choiceID >= m_choices.size()) choiceID = m_choices.size() - 1u;

    m_dropDownList.select(choiceID);

    return true;
}

bool DropDownSelector::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    float yOffset = mousePos.y - m_tableLayout.localPosition().y;
    uint choiceID = static_cast<uint>(yOffset / m_lineHeight);
    if (choiceID >= m_choices.size()) choiceID = m_choices.size() - 1u;

    hoverChoice(choiceID);

    return true;
}

void DropDownSelector::handleMouseLeft()
{
    clearHoveredChoice();
}

//-----------------------------//
//----- Choice management -----//

void DropDownSelector::add(std::wstring string)
{
    auto choiceInfoLabel = std::make_unique<scene::RichLabel>();
    choiceInfoLabel->setText(string);
    choiceInfoLabel->setPrestyle(scene::RichLabel::Prestyle::NUI);
    m_tableLayout.setDimensions(m_choices.size() + 1u, 1u);
    m_tableLayout.setRowAdapt(m_choices.size(), Adapt::FIXED, m_lineHeight);
    m_tableLayout.setChild(m_choices.size(), 0u, *choiceInfoLabel);

    ChoiceInfo choiceInfo;
    choiceInfo.string = std::move(string);
    choiceInfo.label = std::move(choiceInfoLabel);
    m_choices.emplace_back(std::move(choiceInfo));
}

void DropDownSelector::clear()
{
    m_choices.clear();
}

//--------------------//
//----- Hovering -----//

void DropDownSelector::hoverChoice(uint choiceID)
{
    returnif (m_hoveredChoiceID == choiceID);

    // Remove previous hovering
    clearHoveredChoice();

    returnif (choiceID >= m_choices.size());
    m_hoveredChoiceID = choiceID;

    // Set shader effect
    m_choices[choiceID].label->setShader("nui/hover");

    // Set highlight
    float yOffset = choiceID * m_lineHeight + m_tableLayout.localPosition().y;
    setHoverRect({0.4f * m_hPadding, yOffset, size().x - 1.2f * m_hPadding, m_lineHeight});
}

void DropDownSelector::clearHoveredChoice()
{
    returnif (m_hoveredChoiceID == -1u);

    // Remove shader effect
    m_choices[m_hoveredChoiceID].label->setShader("");

    // Remove highlight
    m_hoverHighlight.setFillColor(sf::Color::Transparent);

    m_hoveredChoiceID = -1u;
}

void DropDownSelector::setHoverRect(const sf::FloatRect& rect)
{
    m_hoverHighlight.setFillColor({255u, 255u, 255u, 16u});

    m_hoverHighlight.setPosition(rect.left, rect.top);
    m_hoverHighlight.setSize({rect.width, rect.height});
}

//---------------//
//----- ICU -----//

void DropDownSelector::refresh()
{
    // Content
    m_tableLayout.setColAdapt(0u, Adapt::FIXED, size().x);

    /*// Texts
    float choicePosition = m_vPadding;
    for (auto& choice : m_choices) {
        choice.text.setPosition(m_hPadding, choicePosition);
        choicePosition += m_lineHeight;
    }*/

    // Fill
    m_fillLeft.setPosition(0.f, 0.f);
    m_fillMiddle.setPosition(m_leftWidth, 0.f);
    m_fillRight.setPosition(size().x, 0.f);

    float fillWidth = size().x - (m_leftWidth + m_rightWidth);
    float fillHeight = size().y - m_bottomHeight;
    m_fillLeft.setSize({m_leftWidth, fillHeight});
    m_fillMiddle.setSize({fillWidth, fillHeight});
    m_fillRight.setSize({m_rightWidth, fillHeight});

    // Bottom
    m_bottomLeft.setPosition(0.f, size().y);
    m_bottomMiddle.setPosition(m_leftWidth, size().y);
    m_bottomRight.setPosition(size().x, size().y);

    m_bottomLeft.setSize({m_leftWidth, m_bottomHeight});
    m_bottomMiddle.setSize({fillWidth, m_bottomHeight});
    m_bottomRight.setSize({m_rightWidth, m_bottomHeight});
}
