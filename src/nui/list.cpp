#include "nui/list.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "config/nui.hpp"
#include "sfe/label.hpp"
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

    // Selection highlight
    m_selectionHighlight.setFillColor({255u, 255u, 255u, 32u});

    refreshDisplay();
}

//-------------------//
//----- Routine -----//

void List::update()
{
    m_table.setSize(size());
    returnif (m_columns.empty());

    refreshBordersPosition();
}

void List::refreshDisplay()
{
    config::NUI cNUI;

    m_lineHeight = cNUI.borderThick + cNUI.fontVSpace + 2.f * cNUI.vPadding;
    m_table.setDimensions(0u, m_columns.size(), m_lineHeight);

    // Refresh selection highlight.
    if (!m_lines.empty()) selectLine(m_selectedLine);

    update();
    baseClass::refreshDisplay();
}

//------------------//
//----- Events -----//

void List::handleMouseButtonPressed(const sf::Mouse::Button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    // Do not take first line, they are the columns titles
    uint line = mousePos.y / m_lineHeight - 1u;

    if (line < m_lines.size())
        selectLine(line);
}

void List::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    uint line = mousePos.y / m_lineHeight - 1u;

    for (uint l = 0u; l < m_lines.size(); ++l)
    for (auto& cell : m_lines[l].cells)
        if (l == line) cell.label->setShader(ShaderID::NUI_HOVER);
        else cell.label->setShader(ShaderID::NONE);
}

void List::handleMouseLeft()
{
    for (uint l = 0u; l < m_lines.size(); ++l)
    for (auto& cell : m_lines[l].cells)
        cell.label->setShader(ShaderID::NONE);
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
        m_columns[column].title = std::make_unique<sfe::Label>();
        m_columns[column].title->setText(std::move(title));
        m_columns[column].title->setPrestyle(sfe::Label::Prestyle::NUI_TITLE);
        m_table.setChild(0u, column, *m_columns[column].title);
        ++column;
    }

    refreshBordersPosition();
}

void List::setColumnAdapt(uint index, Adapt adapt)
{
    m_table.setColAdapt(index, adapt);
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

void List::addLine(const std::initializer_list<std::wstring>& values)
{
    massert(values.size() == m_columns.size(), "Expected " << m_columns.size() << " values to match columns number.");

    LineInfo line;
    line.cells.resize(values.size());

    uint row = m_lines.size() + 1u;
    uint column = 0u;
    for (auto& value : values) {
        line.cells[column].label = std::make_unique<sfe::Label>();
        line.cells[column].label->setText(value);
        line.cells[column].label->setPrestyle(sfe::Label::Prestyle::NUI);
        line.cells[column].label->setDetectable(false);
        m_table.setChild(row, column, *line.cells[column].label, m_columns[column].hAlign, m_columns[column].vAlign);
        ++column;
    }
    m_lines.emplace_back(std::move(line));

    // Select the first line when added
    if (m_lines.size() == 1u)
        selectLine(0u);
}

//-------------------------//
//----- Selected line -----//

void List::selectLine(uint line)
{
    assert(line < m_lines.size());
    m_selectedLine = line;

    float yLine = (line + 1u) * m_lineHeight;
    setSelectionRect({0.f, yLine, size().x, m_lineHeight});
}

void List::setSelectionRect(const sf::FloatRect& focusRect)
{
    setFocusRect(focusRect);
    m_selectionHighlight.setPosition(focusRect.left, focusRect.top);
    m_selectionHighlight.setSize({focusRect.width, focusRect.height});
}

//-----------------------------------//
//----- Internal change updates -----//

void List::refreshBordersPosition()
{
    m_hBorders.resize(3u);
    m_vBorders.resize(m_columns.size() + 1u);

    clearParts();
    addPart(&m_selectionHighlight);

    // Vertical
    for (uint c = 0u; c <= m_columns.size(); ++c) {
        m_vBorders[c].setShade(0.1f);
        m_vBorders[c].setLength(size().y - m_lineHeight);
        m_vBorders[c].setPosition(m_table.colOffset(c), m_lineHeight);
        addPart(&m_vBorders[c]);
    }

    // Horizontal
    for (uint r = 0u; r < 3u; ++r) {
        m_hBorders[r].setShade(0.05f);
        m_hBorders[r].setLength(size().x);
        m_hBorders[r].setPosition(0.f, m_table.rowOffset(r));
        addPart(&m_hBorders[r]);
    }

    // Correction for last horizontal
    m_hBorders.back().setPosition(0.f, size().y);
}
