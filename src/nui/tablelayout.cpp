#include "nui/tablelayout.hpp"

#include "tools/tools.hpp"
#include "tools/debug.hpp"
#include "config/nui.hpp"

using namespace nui;

TableLayout::TableLayout()
    : m_hPaddingAuto(true)
    , m_vPaddingAuto(true)
{
    setDetectable(false);

    refreshDisplay();
}

//-------------------//
//----- Routine -----//

void TableLayout::onSizeChanges()
{
    refreshDimensions();
    refreshChildrenPosition();
}

void TableLayout::refreshDisplay()
{
    config::NUI cNUI;

    if (m_hPaddingAuto) m_hPadding = cNUI.hPadding;
    if (m_vPaddingAuto) m_vPadding = cNUI.vPadding;

    refreshDimensions();
    refreshChildrenPosition();
    baseClass::refreshDisplay();
}

//---------------------//
//----- Structure -----//

void TableLayout::setDimensions(uint rows, uint cols, float rowsStep, float colsStep)
{
    m_rowsDimension = rows;
    m_colsDimension = cols;
    m_rowsStep = rowsStep;
    m_colsStep = colsStep;

    refreshDimensions();
}

void TableLayout::setRowAdapt(uint row, Adapt adapt, float param)
{
    assert(row < m_rows.size());

    m_rows[row].adapt = adapt;
    if (adapt == Adapt::FIXED) m_rows[row].height = param;

    refreshChildrenPosition();
}

void TableLayout::setColAdapt(uint col, Adapt adapt, float param)
{
    assert(col < m_cols.size());

    m_cols[col].adapt = adapt;
    if (adapt == Adapt::FIXED) m_cols[col].width = param;

    refreshChildrenPosition();
}

void TableLayout::overridePadding(float hPadding, float vPadding)
{
    m_hPaddingAuto = hPadding < 0.f;
    m_vPaddingAuto = vPadding < 0.f;

    if (!m_hPaddingAuto) m_hPadding = hPadding;
    if (!m_vPaddingAuto) m_vPadding = vPadding;

    refreshDisplay();
}

float TableLayout::colOffset(uint col)
{
    if (col == -1u) col = m_cols.size();
    assert(col < m_cols.size() + 1u);

    float offset = 0.f;
    for (uint c = 0u; c < col; ++c)
        offset += m_cols[c].width;

    return offset;
}

float TableLayout::rowOffset(uint row)
{
    if (row == -1u) row = m_rows.size();
    assert(row < m_rows.size() + 1u);

    float offset = 0.f;
    for (uint r = 0u; r < row; ++r)
        offset += m_rows[r].height;

    return offset;
}

float TableLayout::maxChildHeightInRow(uint row)
{
    float maxHeight = 0.f;

    for (uint c = 0u; c < m_cols.size(); ++c)
        if (m_children.count({row, c}) != 0u)
            maxHeight = std::max(maxHeight, m_children.at({row, c}).entity.size().y + 2.f * m_vPadding);

    return maxHeight;
}

float TableLayout::maxChildWidthInCol(uint col)
{
    float maxWidth = 0.f;

    for (uint r = 0u; r < m_rows.size(); ++r)
        if (m_children.count({r, col}) != 0u)
            maxWidth = std::max(maxWidth, m_children.at({r, col}).entity.size().x + 2.f * m_hPadding);

    return maxWidth;
}

void TableLayout::positionChild(uint row, uint col, float x, float y)
{
    returnif (m_children.count({row, col}) == 0u);
    auto& child = m_children.at({row, col});

    // x coordinates
    if (child.hAlign == Align::STANDARD)
        x += m_hPadding;
    else if (child.hAlign == Align::CENTER)
        x += (m_cols[col].width - child.entity.size().x) / 2.f;
    else if (child.hAlign == Align::OPPOSITE)
        x += m_cols[col].width - child.entity.size().x - m_hPadding;

    // y coordinates
    if (child.vAlign == Align::STANDARD)
        y += m_vPadding;
    else if (child.vAlign == Align::CENTER)
        y += (m_rows[row].height - child.entity.size().y) / 2.f;
    else if (child.vAlign == Align::OPPOSITE)
        y += m_rows[row].height - child.entity.size().y - m_vPadding;

    child.entity.setLocalPosition({x, y});
}

//--------------------//
//----- Children -----//

void TableLayout::setChild(uint row, uint col, scene::Entity& child, Align hAlign, Align vAlign)
{
    assert(row < m_rows.size());
    assert(col < m_cols.size());

    // Remove there was another child here
    removeChild(row, col);

    // Add new child
    attachChild(child);
    ChildInfo childInfo{child, hAlign, vAlign};
    m_children.insert({{row, col}, childInfo});

    // TODO Delay refresh until next updateRoutine() so that multiple adds don't recompute uselessly?
    refreshChildrenPosition();
}

void TableLayout::setChildAlign(uint row, uint col, Align hAlign, Align vAlign)
{
    returnif (m_children.count({row, col}) == 0u);

    m_children.at({row, col}).hAlign = hAlign;
    m_children.at({row, col}).vAlign = vAlign;

    refreshChildrenPosition();
}

void TableLayout::removeChildren()
{
    for (uint r = 0u; r < m_rows.size(); ++r)
    for (uint c = 0u; c < m_cols.size(); ++c)
        removeChild(r, c);
}

void TableLayout::removeChild(uint row, uint col)
{
    returnif (m_children.count({row, col}) == 0u);

    detachChild(m_children.at({row, col}).entity);
    m_children.erase({row, col});

    refreshChildrenPosition();
}

//-----------------------------------//
//----- Internal change updates -----//

void TableLayout::refreshChildrenPosition()
{
    returnif (m_rowsDimension == 0u && m_rowsStep < 0.f);
    returnif (m_colsDimension == 0u && m_colsStep < 0.f);

    // Refresh size
    refreshRowsSize();
    refreshColsSize();

    // Set new positions
    float y = 0.f;
    for (uint r = 0u; r < m_rows.size(); ++r) {
        float x = 0.f;
        for (uint c = 0u; c < m_cols.size(); ++c) {
            positionChild(r, c, x, y);
            x += m_cols[c].width;
        }
        y += m_rows[r].height;
    }
}

void TableLayout::refreshRowsSize()
{
    float heightLeft = size().y;
    uint fillRowsCount = 0u;

    // Set size for all non-fill rows
    for (uint r = 0u; r < m_rows.size(); ++r) {
        auto& row = m_rows[r];

        switch (row.adapt) {
        case Adapt::FILL:
            ++fillRowsCount;
            break;

        case Adapt::FIT:
            row.height = maxChildHeightInRow(r);
            heightLeft -= row.height;
            break;

        case Adapt::FIXED:
            heightLeft -= row.height;
            break;
        }
    }

    // Set size for all fill rows
    float heightHint = heightLeft / fillRowsCount;
    for (auto& row : m_rows)
        if (row.adapt == Adapt::FILL)
            row.height = heightHint;
}

void TableLayout::refreshColsSize()
{
    float widthLeft = size().x;
    uint fillColsCount = 0u;

    // Set size for all non-fill columns
    for (uint c = 0u; c < m_cols.size(); ++c) {
        auto& col = m_cols[c];

        switch (col.adapt) {
        case Adapt::FILL:
            ++fillColsCount;
            break;

        case Adapt::FIT:
            col.width = maxChildWidthInCol(c);
            widthLeft -= col.width;
            break;

        case Adapt::FIXED:
            widthLeft -= col.width;
            break;
        }
    }

    // Set size for all fill columns
    float widthHint = widthLeft / fillColsCount;
    for (auto& col : m_cols)
        if (col.adapt == Adapt::FILL)
            col.width = widthHint;
}

void TableLayout::refreshDimensions()
{
    if ((m_rowsDimension == 0u && m_rowsStep < 0.f)
        || (m_colsDimension == 0u && m_colsStep < 0.f)) {
        m_rows.clear();
        m_cols.clear();
        return;
    }

    // Estimating from step
    uint rows = (m_rowsDimension == 0u)? size().y / m_rowsStep : m_rowsDimension;
    uint cols = (m_colsDimension == 0u)? size().x / m_colsStep : m_colsDimension;

    // Detach the children to be lost
    for (uint r = rows; r < m_rows.size(); ++r)
    for (uint c = cols; c < m_cols.size(); ++c)
        removeChild(r, c);

    // Do the actual resize
    m_rows.resize(rows);
    m_cols.resize(cols);

    // Auto-adapt if fixed
    if (m_rowsDimension == 0u) {
        for (auto& row : m_rows) {
            row.adapt = Adapt::FIXED;
            row.height = m_rowsStep;
        }
    }

    if (m_colsDimension == 0u) {
        for (auto& col : m_cols) {
            col.adapt = Adapt::FIXED;
            col.width = m_colsStep;
        }
    }
}

