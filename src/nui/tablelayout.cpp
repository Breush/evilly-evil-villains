#include "nui/tablelayout.hpp"

#include "tools/tools.hpp"
#include "tools/debug.hpp"
#include "tools/vector.hpp"
#include "config/nuiguides.hpp"

using namespace nui;

TableLayout::TableLayout()
    : m_hPaddingAuto(true)
    , m_vPaddingAuto(true)
{
    setDetectable(false);
}

//-------------------//
//----- Routine -----//

void TableLayout::onSizeChanges()
{
    refreshRowsSize();
    refreshColsSize();
    refreshDimensions();
    refreshChildrenPosition();
}

void TableLayout::onChildSizeChanges(scene::Entity&)
{
    // Note: It's not possible to detect which entity it is, and just refresh this one,
    // because it might change the position dimensions of other rows/cols.
    refreshRowsSize();
    refreshColsSize();
    refreshChildrenPosition();
}

void TableLayout::refreshNUI(const config::NUIGuides& cNUI)
{
    // Be sure all children have there definitive size
    baseClass::refreshNUI(cNUI);

    m_hRefPadding = cNUI.hPadding;
    m_vRefPadding = cNUI.vPadding;

    refreshPaddingAuto();
    refreshRowsSize();
    refreshColsSize();
    refreshDimensions();
    refreshChildrenPosition();
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

    if (m_autoSize && adapt != Adapt::FIT)
        mquit("AutoSize forces all adapt to be FIT.");

    m_rows[row].adapt = adapt;
    if (adapt == Adapt::FIXED) m_rows[row].height = param;

    refreshChildrenPosition();
}

void TableLayout::setColAdapt(uint col, Adapt adapt, float param)
{
    assert(col < m_cols.size());

    if (m_autoSize && adapt != Adapt::FIT)
        mquit("AutoSize forces all adapt to be FIT.");

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

    refreshPaddingAuto();
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
    float ox = 0.f, oy = 0.f;

    // x coordinates
    float dx = m_cols[col].width - child.entity.size().x;
    if (child.hAlign == Align::STANDARD)        ox = m_hPadding;
    else if (child.hAlign == Align::CENTER)     ox = dx / 2.f;
    else if (child.hAlign == Align::OPPOSITE)   ox = dx - m_hPadding;

    // y coordinates
    float dy = m_rows[row].height - child.entity.size().y;
    if (child.vAlign == Align::STANDARD)        oy = m_vPadding;
    else if (child.vAlign == Align::CENTER)     oy = dy / 2.f;
    else if (child.vAlign == Align::OPPOSITE)   oy = dy - m_vPadding;

    child.entity.setLocalPosition({x + ox, y + oy});

    // Clip child if too big
    if (m_childrenClipping) {
        sf::FloatRect clipArea{0.f, 0.f, -1.f, -1.f};
        if (ox + m_hPadding > dx) clipArea.width  = m_cols[col].width  - (ox + m_hPadding);
        if (oy + m_vPadding > dy) clipArea.height = m_rows[row].height - (oy + m_vPadding);
        if (clipArea.width >= 0.f && clipArea.height < 0.f) clipArea.height = child.entity.size().y;
        if (clipArea.width < 0.f && clipArea.height >= 0.f) clipArea.width  = child.entity.size().x;
        child.entity.setClipArea(clipArea);
    }
}

void TableLayout::setAutoSize(bool autoSize)
{
    m_autoSize = autoSize;
    updateSize();
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

    // If and only if we have a fit adapt column
    refreshDimensions();
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

void TableLayout::updateSize()
{
    returnif (!m_autoSize);
    sf::Vector2f currentSize;

    for (const auto& col : m_cols)
        currentSize.x += col.width;

    for (const auto& row : m_rows)
        currentSize.y += row.height;

    setSize(currentSize);
}

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

    updateSize();
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

    updateSize();
}

void TableLayout::refreshDimensions()
{
    if ((m_rowsDimension == 0u && m_rowsStep < 0.f)
        || (m_colsDimension == 0u && m_colsStep < 0.f)) {
        m_rows.clear();
        m_cols.clear();
        return;
    }

    if (m_autoSize && (m_rowsDimension == 0u || m_colsDimension == 0u))
        mquit("AutoSize demands precise table dimensions.");

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

    // Remove Adapt::FILL if auto-size
    if (m_autoSize) {
        for (auto& row : m_rows)
            row.adapt = Adapt::FIT;

        for (auto& col : m_cols)
            col.adapt = Adapt::FIT;
    }
}

void TableLayout::refreshPaddingAuto()
{
    if (m_hPaddingAuto) m_hPadding = m_hRefPadding;
    if (m_vPaddingAuto) m_vPadding = m_vRefPadding;
}

