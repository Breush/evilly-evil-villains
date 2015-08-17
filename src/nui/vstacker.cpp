#include "nui/vstacker.hpp"

#include "tools/tools.hpp"
#include "config/nuiguides.hpp"

using namespace nui;

VStacker::VStacker()
{
}

//-------------------//
//----- Routine -----//

void VStacker::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_padding = cNUI.vPadding;

    updateSize();
}

void VStacker::onSizeChanges()
{
    returnif (m_children.empty());

    refreshChildrenPositions();
}

void VStacker::onChildSizeChanges(scene::Entity&)
{
    updateSize();
}

void VStacker::updateSize()
{
    if (m_children.empty()) {
        setSize({0.f, 0.f});
        return;
    }

    // Max width and sum height
    float width = 0.f;
    float height = 0.f;

    for (auto& child : m_children) {
        width = std::max(width, child.entity->size().x);
        height += child.entity->size().y;
    }

    // Add padding to height
    height += (m_children.size() - 1u) * m_padding;

    setSize({width, height});
}

//-----------------------//
//----- Positioning -----//

void VStacker::refreshChildrenPositions()
{
    // Children positions
    float y = 0.f;

    // Setting children positions
    for (auto& child : m_children) {
        // Horizontal alignment
        float x = getX(child.entity->size().x, child.align);

        // Setting position
        child.entity->setRelativeOrigin({0.f, 0.f});
        child.entity->setLocalPosition({x, y});

        // Vertical post-alignment
        y += child.entity->size().y + m_padding;
    }
}

float VStacker::getX(float childWidth, Align inAlign)
{
    // Center
    if (inAlign == Align::CENTER)
        return (size().x - childWidth) / 2.f;

    // Opposite : right
    else if (inAlign == Align::OPPOSITE)
        return size().x - childWidth;

    // Standard : left
    return 0.f;
}
