#include "core/application.hpp"

#include "nui/list.hpp"
#include "nui/uicore.hpp"

#include "tools/debug.hpp"
#include "tools/tools.hpp"

#include "resources/holder.hpp"

using namespace nui;

List::List()
    : baseClass()
    , m_hPadding(6)
    , m_vPadding(6)
    , m_borderThick(1)
    , m_sbWidth(31)
    , m_lineHeight(32)
    , m_linesCount(0)
{
    // TODO #3 - Use config parameter to determine size
}

void List::init()
{
}

void List::update()
{
    clearParts();

    // Columns
    uint borderColumnHeight = linesCount() * lineHeight();
    for (uint i = 0; i < m_columns.size(); ++i) {
        auto& column = m_columns[i];
        uint y = vPadding() + borderThick();
        uint x = column.x + hPadding() + borderThick();

        // Border
        m_vBorders[i].setLength(borderColumnHeight);
        m_vBorders[i].setPosition(column.x, lineHeight());
        addPart(&m_vBorders[i]);

        // Column name position
        column.text.setPosition(x, y);
        addPart(&column.text);

        // Lines
        y += lineHeight();
        for (auto& line : m_lines) {
            line[i].text.setPosition(x, y);
            addPart(&line[i].text);

            // Clipping
            if (column.clip && line[i].textWidth > column.width - hPadding()) {
                line[i].clippingRect = sf::IntRect(column.x, y - vPadding(), column.width - hPadding(), lineHeight());
                setPartClippingRect(&line[i].text, &line[i].clippingRect);
            }

            y += lineHeight();
        }
    }

    uint vBorderLast = m_columns.size();
    m_vBorders[vBorderLast].setLength(borderColumnHeight);
    m_vBorders[vBorderLast].setPosition(size().x - sbWidth(), lineHeight());
    addPart(&m_vBorders[vBorderLast]);

    // Horizontal borders
    uint y = 0;

    for (uint i = 0; i <= 1; ++i) {
        m_hBorders[i].setLength(size().x);
        m_hBorders[i].setPosition(0, y);
        addPart(&m_hBorders[i]);
        y += lineHeight();
    }

    for (uint i = 2; i <= linesCount(); ++i) {
        m_hBorders[i].setLength(size().x - 2 * sbWidth());
        m_hBorders[i].setPosition(sbWidth(), y);
        //addPart(&m_hBorders[i]);
        y += lineHeight();
    }

    uint hBorderLast = linesCount() + 1;
    m_hBorders[hBorderLast].setLength(size().x);
    m_hBorders[hBorderLast].setPosition(0, y);
    addPart(&m_hBorders[hBorderLast]);
}

//-------------------//
//----- Columns -----//

void List::setColumns(std::initializer_list<std::wstring> columns)
{
    m_columns.clear();

    sf::Text text;
    sf::Font& font = Application::context().fonts.get(Fonts::NUI);
    text.setCharacterSize(16);
    text.setFont(font);

    for (auto& column : columns) {
        text.setString(column);
        text.setStyle(sf::Text::Bold);
        m_columns.push_back( {text, 0, 0, true, true});
    }

    m_vBorders.resize(m_columns.size() + 1);
    updateColumnInfos();
}

void List::setColumnFillClip(uint index, bool fill, bool clip)
{
    m_columns[index].fill = fill;
    m_columns[index].clip = clip;
}

uint List::getColumnWidthMax(uint index)
{
    auto bounds = m_columns[index].text.getLocalBounds();
    uint columnSpace = 2 * hPadding() + borderThick();
    uint widthMax = bounds.left + bounds.width + columnSpace;

    // Lines
    for (auto& line : m_lines) {
        uint lineWidth = line[index].textWidth + columnSpace;
        if (widthMax < lineWidth)
            widthMax = lineWidth;
    }

    return widthMax;
}

uint List::getColumnWidthHint()
{
    uint widthHint = size().x - borderThick() - 2 * sbWidth();
    uint nFillOrClip = 0;

    // No fill, no clip reduce space
    for (uint i = 0; i < m_columns.size(); ++i) {
        if (!m_columns[i].fill && !m_columns[i].clip)
            widthHint -= getColumnWidthMax(i);
        else
            ++nFillOrClip;
    }

    // Nothing to fill or clip
    returnif (nFillOrClip == 0) 0;

    return widthHint / nFillOrClip;
}

//-----------------//
//----- Lines -----//

void List::addLine(std::initializer_list<std::wstring> values)
{
    massert(values.size() == m_columns.size(), "List - expected " << m_columns.size() << " values");

    sf::Text text;
    sf::Font& font = Application::context().fonts.get(Fonts::NUI);
    text.setCharacterSize(16);
    text.setFont(font);

    std::vector<LineInfo> line;
    for (auto& value : values) {
        text.setString(value);
        auto bounds = text.getLocalBounds();
        uint width = bounds.left + bounds.width;
        uint height = bounds.top + bounds.height;
        line.push_back( {text, width, height, sf::IntRect(0, 0, width, height)});
    }
    m_lines.push_back(line);

    updateColumnInfos();
}

//------------------------//
//----- Mouse events -----//

void List::handleMouseEvent(const sf::Event& event, const sf::Vector2f& relPos)
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

void List::handleMousePressed(const sf::Event& event, const sf::Vector2f& relPos)
{
    sf::Vector2f pos = getInverseTransform().transformPoint(relPos.x, relPos.y);

    // TODO Select row
    uint line = pos.y/lineHeight() - 1;
    if (line < linesCount())
        setFocusedLine(line);
}

void List::handleMouseMoved(const sf::Event&, const sf::Vector2f&)
{
    //uint x = getInverseTransform().transformPoint(event.mouseMove.x, event.mouseMove.y).x;
    // TODO Hovering parts
}

void List::handleMouseLeft()
{
    // Resetting hovering
    resetPartsShader();
}

//---------------------------//
//----- Keyboard events -----//

/*void List::handleKeyboardEvent(const sf::Event&)
{
}*/

//-------------------//
//----- Updates -----//

void List::changedSize()
{
    m_linesCount = size().y / lineHeight() - 1;
    m_hBorders.resize(linesCount() + 2);

    baseClass::changedSize();
}

//-----------------------------//
//----- Focus interaction -----//

void List::setFocusedLine(uint line)
{
    uint yLine = (line + 1) * lineHeight();
    setFocusRect(sf::FloatRect(0, yLine, size().x, lineHeight()));
    setStatus(true);
}

//------------------//
//----- Visual -----//

void List::updateColumnInfos()
{
    uint x = sbWidth();
    uint columnHint = getColumnWidthHint();

    // Setting correct size
    for (uint i = 0; i < m_columns.size(); ++i) {
        uint columnMax = getColumnWidthMax(i);
        m_columns[i].x = x;

        // Give fill or clip correct width
        if (columnMax <= columnHint)
            m_columns[i].width = (m_columns[i].fill)? columnHint : columnMax;
        else
            m_columns[i].width = (m_columns[i].clip)? columnHint : columnMax;

        x += m_columns[i].width;
    }

    update();
}
