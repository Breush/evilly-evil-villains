#include "sfe/grid.hpp"

#include "sfe/line.hpp"
#include "tools/tools.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

using namespace sfe;

Grid::Grid()
    : m_rows(0)
    , m_columns(0)
    , m_size(0.f, 0.f)
    , m_visible(true)
{
}

//-------------------//
//----- Routine -----//

void Grid::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    returnif (!m_visible);

    // Transform from sf::Tranformable
    states.transform *= getTransform();

    // Drawing
    for (auto& vBorder : m_vBorders)
        target.draw(vBorder, states);

    for (auto& hBorder : m_hBorders)
        target.draw(hBorder, states);
}

//-------------------//
//----- Updates -----//

void Grid::update()
{
    m_vBorders.resize(m_columns + 1);
    m_hBorders.resize(m_rows + 1);

    float cellHeight = m_size.y / m_rows;
    float cellWidth = m_size.x / m_columns;

    for (uint j = 0; j <= m_rows; ++j) {
        m_hBorders[j].setShade(0);
        m_hBorders[j].setLength(m_size.x);
        m_hBorders[j].setPosition(0.f, j * cellHeight);
    }

    for (uint i = 0; i <= m_columns; ++i) {
        m_vBorders[i].setShade(0);
        m_vBorders[i].setLength(m_size.y);
        m_vBorders[i].setPosition(i * cellWidth, 0);
    }
}

//-----------------------//
//----- Interaction -----//

void Grid::setRowsColumns(const uint& inRows, const uint& inColumns)
{
    m_rows = inRows;
    m_columns = inColumns;
    update();
}

sf::Vector2u Grid::rowColumnFromCoords(const sf::Vector2f& coords) const
{
    if (coords.x < 0 || coords.x >= m_size.x || coords.y < 0 || coords.y >= m_size.y)
        return sf::Vector2u(-1, -1);

    float cellHeight = m_size.y / m_rows;
    float cellWidth = m_size.x / m_columns;

    return sf::Vector2u(coords.y / cellHeight, coords.x / cellWidth);
}

sf::Vector2f Grid::cellPosition(uint row, uint column) const
{
    return sf::Vector2f(column * cellSize().x, row * cellSize().y);
}

sf::Vector2f Grid::cellSize() const
{
    return sf::Vector2f(m_size.x / m_columns, m_size.y / m_rows);
}
