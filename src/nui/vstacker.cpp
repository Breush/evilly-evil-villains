#include "nui/vstacker.hpp"

#include "tools/debug.hpp"
#include "tools/int.hpp"
#include "config/nui.hpp"

using namespace nui;

VStacker::VStacker()
{
    refreshDisplay();
}

//-------------------//
//----- Routine -----//

void VStacker::refreshDisplay()
{
    config::NUI cNUI;

    m_padding = cNUI.vPadding;

    update();
    baseClass::refreshDisplay();
}

//------------------//
//----- Visual -----//

void VStacker::update()
{
    if (m_children.empty())
        return;

    // Children positions
    float y = getInitY();

    // Setting children positions
    for (auto& childInfo : m_children) {
        // Horizontal alignment
        float x = getX(childInfo.entity->size().x, childInfo.align);

        // Vertical pre-alignment
        y += getPreY(childInfo.entity->size().y);

        // Setting position
        childInfo.entity->setLocalPosition({x, y});

        // Vertical post-alignment
        y += getPostY(childInfo.entity->size().y);
    }
}

inline float VStacker::getX(float childWidth, Align inAlign)
{
    // Max width
    float width = 0.f;
    for (auto& childInfo : m_children)
        width = std::max(width, childInfo.entity->size().x);

    // Center
    if (inAlign == Stacker::Align::CENTER)
        return (size().x - childWidth) / 2.f;

    // Opposite : right
    else if (inAlign == Stacker::Align::OPPOSITE)
        return (size().x - m_padding) - (width + childWidth) / 2.f;

    // Standard : left
    return m_padding + (width - childWidth) / 2;
}

inline float VStacker::getInitY()
{
    // Center
    if (align() == Stacker::Align::CENTER) {
        float height = (m_children.size() - 1) * m_padding;
        for (auto& childInfo : m_children)
            height += childInfo.entity->size().y;
        return (size().y - height) / 2;
    }

    // Opposite : bottom
    else if (align() == Stacker::Align::OPPOSITE)
        return size().y - m_padding;

    // Standard : top
    return m_padding;
}

inline float VStacker::getPreY(float childHeight)
{
    // Opposite
    if (align() == Stacker::Align::OPPOSITE)
        return -childHeight;

    // Center or Standard
    return 0.f;
}

inline float VStacker::getPostY(float childHeight)
{
    // Opposite
    if (align() == Stacker::Align::OPPOSITE)
        return -m_padding;

    // Center or Standard
    return childHeight + m_padding;
}
