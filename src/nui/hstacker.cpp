#include "nui/hstacker.hpp"

#include "tools/debug.hpp"
#include "tools/int.hpp"
#include "config/nui.hpp"

using namespace nui;

HStacker::HStacker()
{
    refreshDisplay();
}

//-------------------//
//----- Routine -----//

void HStacker::refreshDisplay()
{
    config::NUI cNUI;

    m_padding = cNUI.hPadding;

    update();
    baseClass::refreshDisplay();
}

//------------------//
//----- Visual -----//

void HStacker::update()
{
    if (m_children.empty())
        return;

    // Children positions
    float x = getInitX();

    // Setting children positions
    for (auto& childInfo : m_children) {
        // Vertical alignment
        float y = getY(childInfo.entity->size().y, childInfo.align);

        // Horizontal pre-alignment
        x += getPreX(childInfo.entity->size().x);

        // Setting position
        childInfo.entity->setLocalPosition({x, y});

        // Horizontal post-alignment
        x += getPostX(childInfo.entity->size().x);
    }
}

inline float HStacker::getY(float childHeight, Align inAlign)
{
    // Max height
    float height = 0.f;
    for (auto& childInfo : m_children)
        height = std::max(height, childInfo.entity->size().y);

    // Center
    if (inAlign == Stacker::Align::CENTER)
        return (size().y - childHeight) / 2.f;

    // Opposite : bottom
    else if (inAlign == Stacker::Align::OPPOSITE)
        return (size().y - m_padding) - (height + childHeight) / 2.f;

    // Standard : top
    return m_padding + (height - childHeight) / 2.f;
}

inline float HStacker::getInitX()
{
    // Center
    if (align() == Stacker::Align::CENTER) {
        float width = (m_children.size() - 1) * m_padding;
        for (auto& childInfo : m_children)
            width += childInfo.entity->size().x;
        return (size().x - width) / 2.f;
    }

    // Opposite : right
    else if (align() == Stacker::Align::OPPOSITE)
        return size().x - m_padding;

    // Standard : left
    return m_padding;
}

inline float HStacker::getPreX(float childWidth)
{
    // Opposite
    if (align() == Stacker::Align::OPPOSITE)
        return -childWidth;

    // Center or Standard
    return 0.f;
}

inline float HStacker::getPostX(float childWidth)
{
    // Opposite
    if (align() == Stacker::Align::OPPOSITE)
        return -m_padding;

    // Center or Standard
    return childWidth + m_padding;
}
