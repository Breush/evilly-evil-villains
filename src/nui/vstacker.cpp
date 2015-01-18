#include "nui/vstacker.hpp"

#include "tools/debug.hpp"

using namespace nui;

#if 0
VStacker::VStacker(Object* parent, uiCore* core)
    : baseClass( parent, core )
{
}

//------------------//
//----- Visual -----//

void VStacker::recomputeChildrenPos()
{
    if (m_children.empty())
        return;

    // Children positions
    uint x;
    uint y = getInitY();

    // Setting children positions
    for (auto& childInfo : m_children) {
        // Horizontal alignment
        x = getX(childInfo.child->getSize().x, childInfo.align);

        // Vertical pre-alignment
        y += getPreY(childInfo.child->getSize().y);

        // Setting position
        childInfo.child->setPos(sf::Vector2u(x, y));

        // Vertical post-alignment
        y += getPostY(childInfo.child->getSize().y);
    }
}

inline uint VStacker::getX(uint& childWidth, Align& align)
{
    // Max width
    uint width = 0;
    for (auto& childInfo : m_children)
        width = std::max(width, childInfo.child->getSize().x);

    // Center
    if (align == Stacker::CENTER) {
        return (getSize().x - childWidth) / 2;
    }

    // Opposite : right
    else if (align == Stacker::OPPOSITE) {
        return (getSize().x - m_margin) - (width + childWidth) / 2;
    }

    // Standard : left
    return m_margin + (width - childWidth) / 2;
}

inline uint VStacker::getInitY()
{
    // Center
    if (m_align == Stacker::CENTER) {
        uint height = (m_children.size() - 1) * m_margin;
        for (auto& childInfo : m_children)
            height += childInfo.child->getSize().y;
        return (getSize().y - height) / 2;
    }

    // Opposite : bottom
    else if (m_align == Stacker::OPPOSITE) {
        return getSize().y - m_margin;
    }

    // Standard : top
    return m_margin;
}

inline int VStacker::getPreY(uint& childHeight)
{
    // Opposite
    if (m_align == Stacker::OPPOSITE) {
        return -childHeight;
    }

    // Center or Standard
    return 0;
}

inline int VStacker::getPostY(uint& childHeight)
{
    // Opposite
    if (m_align == Stacker::OPPOSITE) {
        return -m_margin;
    }

    // Center or Standard
    return childHeight + m_margin;
}
#endif
