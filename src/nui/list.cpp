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

    // Highlight
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

    m_headerOffset = 3.f * cNUI.fontHSpace;
    m_lineHeight = cNUI.borderThick + cNUI.fontVSpace + 2.f * cNUI.vPadding;
    m_table.setDimensions(0u, m_columns.size(), m_lineHeight);

    // Refresh selection highlight
    if (!m_lines.empty()) selectLine(m_selectedLine);

    refreshBordersPosition();
}

//------------------//
//----- Events -----//

void List::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left);

    // Do not take first line, they are the columns titles
    uint line = mousePos.y / m_lineHeight - 1u;
    returnif (line >= m_lines.size());

    // Double-click?
    if (m_doubleClickDelay >= 0.f) {
        returnif (m_callback == nullptr);
        Application::context().sounds.play("select");
        m_callback();
    }
    else {
        m_doubleClickDelay = 0.f;
        Application::context().sounds.play("select");
        selectLine(line);
    }
}

void List::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    uint line = mousePos.y / m_lineHeight - 1u;
    hoverLine(line);
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
        Application::context().sounds.play("select");
        m_callback();
    }
    // Up
    else if (event.key.code == sf::Keyboard::Up) {
        if (m_selectedLine == 0u) {
            Application::context().sounds.play("refuse");
        }
        else {
            Application::context().sounds.play("select");
            selectLine(m_selectedLine - 1u);
        }
        return true;
    }
    // Down
    else if (event.key.code == sf::Keyboard::Down) {
        if (m_selectedLine == m_lines.size() - 1u) {
            Application::context().sounds.play("refuse");
        }
        else {
            Application::context().sounds.play("select");
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

    // Set newly hovered line
    if (line < m_lines.size()) {
        m_hoveredLine = line;

        // Set shader effect
        for (auto& cell : m_lines[line].cells)
            cell.label->setShader("nui/hover");

        // Set highlight
        float yLine = (line + 1u) * m_lineHeight;
        setHoverRect({0.f, yLine, size().x, m_lineHeight});
    }
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

    float yLine = (line + 1u) * m_lineHeight;
    setSelectionRect({0.f, yLine, size().x, m_lineHeight});
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

    m_hBorders.resize(3u);
    m_vBorders.resize(m_columns.size() + 1u);

    clearParts();
    addPart(&m_selectionHighlight);
    addPart(&m_hoverHighlight);

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

    // Header offset
    m_hBorders[0].move(-m_headerOffset, 0.f);
    m_hBorders[1].move(-m_headerOffset, 0.f);
    m_hBorders[0].setLength(m_hBorders[0].length() + 2.f * m_headerOffset);
    m_hBorders[1].setLength(m_hBorders[1].length() + 2.f * m_headerOffset);

    // Correction for last horizontal
    m_hBorders.back().setPosition(0.f, size().y);
}
