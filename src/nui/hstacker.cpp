#include "nui/hstacker.hpp"

#include "tools/tools.hpp"
#include "tools/vector.hpp"
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

    m_padding = cNUI.vPadding;

    updateSize();
    baseClass::refreshDisplay();
}

void HStacker::onSizeChanges()
{
    refreshChildrenPositions();
}

void HStacker::onChildSizeChanges()
{
    updateSize();
}

void HStacker::updateSize()
{
    // Base case: no child
    if (m_children.empty()) {
        setSize({0.f, 0.f});
        return;
    }

    // Max width and sum height
    float width = 0.f;
    float height = 0.f;

    for (auto& child : m_children) {
        width += child.entity->size().x;
        height = std::max(height, child.entity->size().y);
    }

    // Add padding to height
    width += (m_children.size() - 1u) * m_padding;

    setSize({width, height});
}

//-----------------------//
//----- Positioning -----//

void HStacker::refreshChildrenPositions()
{
    // Children positions
    float x = 0.f;

    // Setting children positions
    for (auto& child : m_children) {
        // Vertical alignment
        float y = getY(child.entity->size().y, child.align);

        // Setting position
        child.entity->setRelativeOrigin({0.f, 0.f});
        child.entity->setLocalPosition({x, y});

        // Horizontal post-alignment
        x += child.entity->size().x + m_padding;
    }
}

float HStacker::getY(float childHeight, Align inAlign)
{
    // Center
    if (inAlign == Align::CENTER)
        return (size().y - childHeight) / 2.f;

    // Opposite : bottom
    else if (inAlign == Align::OPPOSITE)
        return size().y - childHeight;

    // Standard : top
    return 0.f;
}
