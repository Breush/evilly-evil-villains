#include "nui/list.hpp"

#include "core/application.hpp"
#include "tools/debug.hpp"
#include "tools/tools.hpp"
#include "tools/platform-fixes.hpp" // make_unique

using namespace nui;

List::List()
{
    setFocusable(true);
    setFocusOwned(true);

    // Table
    attachChild(m_table);
    m_table.overridePadding(-1.f, 0.f);

    // Header
    const auto& headerLeftTexture = Application::context().textures.get("core/nui/list/header_left");
    const auto& headerRightTexture = Application::context().textures.get("core/nui/list/header_right");
    const auto& headerMiddleTexture = Application::context().textures.get("core/nui/list/header_middle");

    m_headerLeft.setTexture(headerLeftTexture);
    m_headerRight.setTexture(headerRightTexture);
    m_headerMiddle.setTexture(&headerMiddleTexture);

    m_headerMiddle.setPosition(headerLeftTexture.getSize().x, 0.f);
    m_headerRight.setOrigin(headerRightTexture.getSize().x, 0.f);

    m_headerLeftSize = sf::v2f(headerLeftTexture.getSize());

    // Fill
    const auto& fillLeftTexture = Application::context().textures.get("core/nui/list/fill_left");
    const auto& fillRightTexture = Application::context().textures.get("core/nui/list/fill_right");
    const auto& fillMiddleTexture = Application::context().textures.get("core/nui/list/fill_middle");

    m_fillLeft.setTexture(&fillLeftTexture);
    m_fillRight.setTexture(&fillRightTexture);
    m_fillMiddle.setTexture(&fillMiddleTexture);

    m_fillRight.setOrigin(fillRightTexture.getSize().x, 0.f);

    m_fillMiddleSize = sf::v2f(fillMiddleTexture.getSize());

    // Bottom
    const auto& bottomLeftTexture = Application::context().textures.get("core/nui/list/bottom_left");
    const auto& bottomRightTexture = Application::context().textures.get("core/nui/list/bottom_right");
    const auto& bottomMiddleTexture = Application::context().textures.get("core/nui/list/bottom_middle");

    m_footerLeft.setTexture(&bottomLeftTexture);
    m_footerRight.setTexture(&bottomRightTexture);
    m_footerMiddle.setTexture(&bottomMiddleTexture);

    m_footerLeft.setOrigin(0.f, bottomLeftTexture.getSize().y);
    m_footerMiddle.setOrigin(0.f, bottomMiddleTexture.getSize().y);
    m_footerRight.setOrigin(bottomRightTexture.getSize().x, bottomRightTexture.getSize().y);

    m_footerRightSize = sf::v2f(bottomRightTexture.getSize());

    // Highlights
    m_selectionHighlight.setFillColor({255u, 255u, 255u, 32u});
    m_hoverHighlight.setFillColor(sf::Color::Transparent);
}

//-------------------//
//----- Routine -----//

void List::updateRoutine(const sf::Time& dt)
{
    // Refresh double-click
    returnif (m_doubleClickDelay < 0.f);
    if ((m_doubleClickDelay += dt.asSeconds()) > 0.25f)
        m_doubleClickDelay = -1.f;
}

void List::onSizeChanges()
{
    m_table.setSize(size());

    refreshBordersPosition();
}

void List::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_hPadding = cNUI.hPadding;

    m_lineHeight = cNUI.borderThick + cNUI.fontVSpace + 2.f * cNUI.vPadding;
    m_table.setDimensions(0u, m_columns.size(), m_lineHeight);

    // Refresh selection highlight
    if (!m_lines.empty()) selectLine(m_selectedLine);

    refreshBordersPosition();
}

//------------------//
//----- Events -----//

bool List::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left) false;

    // Do not take first line, they are the columns titles
    uint line = mousePos.y / m_lineHeight - 1u;
    returnif (line >= m_lines.size()) false;

    // Double-click?
    if (m_doubleClickDelay >= 0.f) {
        returnif (m_callback == nullptr) true;
        Application::context().sounds.play("core/nui/select/select");
        m_callback();
    }
    else {
        m_doubleClickDelay = 0.f;
        Application::context().sounds.play("core/nui/select/select");
        selectLine(line);
    }

    return true;
}

bool List::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    uint line = mousePos.y / m_lineHeight - 1u;
    hoverLine(line);
    return true;
}

void List::handleMouseLeft()
{
    clearHoveredLine();
}

bool List::handleKeyboardEvent(const sf::Event& event)
{
    returnif (event.type != sf::Event::KeyPressed) false;

    // Confirm
    if (event.key.code == sf::Keyboard::Return) {
        returnif (m_lines.empty() || m_callback == nullptr) false;
        Application::context().sounds.play("core/nui/select/select");
        m_callback();
    }
    // Up
    else if (event.key.code == sf::Keyboard::Up) {
        if (m_selectedLine == 0u) {
            Application::context().sounds.play("core/nui/refuse/refuse");
        }
        else {
            Application::context().sounds.play("core/nui/select/select");
            selectLine(m_selectedLine - 1u);
        }
        return true;
    }
    // Down
    else if (event.key.code == sf::Keyboard::Down) {
        if (m_selectedLine == m_lines.size() - 1u) {
            Application::context().sounds.play("core/nui/refuse/refuse");
        }
        else {
            Application::context().sounds.play("core/nui/select/select");
            selectLine(m_selectedLine + 1u);
        }
        return true;
    }

    return false;
}

//-------------------//
//----- Columns -----//

void List::setColumnsTitles(const std::initializer_list<std::wstring>& titles)
{
    m_table.setDimensions(0u, titles.size(), m_lineHeight);
    m_columns.resize(titles.size());

    uint column = 0u;
    for (auto& title : titles) {
        // Create new label and add to table
        m_columns[column].title = std::make_unique<scene::Label>();
        m_columns[column].title->setText(std::move(title));
        m_columns[column].title->setPrestyle(scene::Label::Prestyle::NUI_TITLE);
        m_table.setChild(0u, column, *m_columns[column].title);
        ++column;
    }

    refreshBordersPosition();
}

void List::setColumnAdapt(uint index, Adapt adapt)
{
    m_table.setColAdapt(index, adapt);
    refreshBordersPosition();
}

void List::setColumnAlign(uint index, Align hAlign, Align vAlign)
{
    assert(index < m_columns.size());

    m_columns[index].hAlign = hAlign;
    m_columns[index].vAlign = vAlign;

    for (uint row = 0u; row <= m_lines.size(); ++row)
        m_table.setChildAlign(row, index, hAlign, vAlign);
}

//-----------------//
//----- Lines -----//

void List::clearLines()
{
    m_selectedLine = -1u;

    // Do not remove the first line...
    for (uint l = 1u; l <= m_lines.size(); ++l)
    for (uint c = 0u; c < m_columns.size(); ++c)
        m_table.removeChild(l, c);

    m_lines.clear();
}

void List::addLine(const std::initializer_list<std::wstring>& values)
{
    massert(values.size() == m_columns.size(), "Expected " << m_columns.size() << " values to match columns number.");

    LineInfo line;
    line.cells.resize(values.size());

    uint row = m_lines.size() + 1u;
    uint column = 0u;
    for (auto& value : values) {
        line.cells[column].label = std::make_unique<scene::Label>();
        line.cells[column].label->setText(value);
        line.cells[column].label->setPrestyle(scene::Label::Prestyle::NUI);
        line.cells[column].label->setDetectable(false);
        m_table.setChild(row, column, *line.cells[column].label, m_columns[column].hAlign, m_columns[column].vAlign);
        ++column;
    }
    m_lines.emplace_back(std::move(line));

    // Select the first line when added
    if (m_lines.size() == 1u)
        selectLine(0u);
}

//------------------------//
//----- Hovered line -----//

void List::hoverLine(uint line)
{
    returnif (m_hoveredLine == line);

    // Remove previous hovering
    clearHoveredLine();

    returnif (line >= m_lines.size());
    m_hoveredLine = line;

    // Set shader effect
    for (auto& cell : m_lines[line].cells)
        cell.label->setShader("core/nui/hover/hover");

    // Set highlight
    float yOffset = (line + 1u) * m_lineHeight;
    setHoverRect({0.4f * m_hPadding, yOffset, size().x - 1.2f * m_hPadding, m_lineHeight});
}

void List::clearHoveredLine()
{
    returnif (m_hoveredLine == -1u);

    // Remove shader effect
    for (auto& cell : m_lines[m_hoveredLine].cells)
        cell.label->setShader("");

    // Remove highlight
    m_hoverHighlight.setFillColor(sf::Color::Transparent);

    m_hoveredLine = -1u;
}

void List::setHoverRect(const sf::FloatRect& rect)
{
    m_hoverHighlight.setFillColor({255u, 255u, 255u, 16u});

    m_hoverHighlight.setPosition(rect.left, rect.top);
    m_hoverHighlight.setSize({rect.width, rect.height});
}

//-------------------------//
//----- Selected line -----//

void List::selectLine(uint line)
{
    assert(line < m_lines.size());
    m_selectedLine = line;

    float yOffset = (line + 1u) * m_lineHeight;
    setSelectionRect({0.4f * m_hPadding, yOffset, size().x - 1.2f * m_hPadding, m_lineHeight});
}

void List::setSelectionRect(const sf::FloatRect& rect)
{
    setFocusRect(rect);
    m_selectionHighlight.setPosition(rect.left, rect.top);
    m_selectionHighlight.setSize({rect.width, rect.height});
}

//-----------------------------------//
//----- Internal change updates -----//

void List::refreshBordersPosition()
{
    returnif (!m_columns.size());

    m_vBorders.resize(m_columns.size() + 1u);

    clearParts();

    // Header
    addPart(&m_headerLeft);
    addPart(&m_headerRight);
    addPart(&m_headerMiddle);

    float fillWidth = size().x - m_headerLeftSize.x - m_footerRightSize.x;
    m_headerRight.setPosition({size().x, 0.f});
    m_headerMiddle.setSize({fillWidth, m_lineHeight});
    m_headerLeft.setScale(1.f, m_lineHeight / m_headerLeftSize.y);
    m_headerRight.setScale(1.f, m_lineHeight / m_headerLeftSize.y);

    // Fill
    addPart(&m_fillLeft);
    addPart(&m_fillRight);
    addPart(&m_fillMiddle);

    float fillHeight = size().y - m_lineHeight - m_footerRightSize.y;
    m_fillLeft.setPosition(0.f, m_lineHeight);
    m_fillRight.setPosition(size().x, m_lineHeight);
    m_fillMiddle.setPosition(m_headerLeftSize.x, m_lineHeight);
    m_fillLeft.setSize({m_headerLeftSize.x, fillHeight});
    m_fillRight.setSize({m_headerLeftSize.x, fillHeight});
    m_fillMiddle.setSize({fillWidth, fillHeight});

    // Bottom
    addPart(&m_footerLeft);
    addPart(&m_footerRight);
    addPart(&m_footerMiddle);

    m_footerLeft.setPosition({0.f, size().y});
    m_footerRight.setPosition({size().x, size().y});
    m_footerMiddle.setPosition({m_headerLeftSize.x, size().y});
    m_footerLeft.setSize(m_footerRightSize);
    m_footerRight.setSize(m_footerRightSize);
    m_footerMiddle.setSize({fillWidth, m_footerRightSize.y});

    // Vertical
    for (uint c = 1u; c <= m_columns.size() - 1u; ++c) {
        m_vBorders[c].setShade(0.1f);
        m_vBorders[c].setLength(size().y - m_lineHeight);
        m_vBorders[c].setPosition(m_table.colOffset(c), m_lineHeight);
        addPart(&m_vBorders[c]);
    }

    // Highlights
    addPart(&m_selectionHighlight);
    addPart(&m_hoverHighlight);
}
