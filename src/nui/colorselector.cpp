#include "nui/colorselector.hpp"

#include "config/nuiguides.hpp"
#include "tools/tools.hpp"

using namespace nui;

ColorSelector::ColorSelector()
{
    // Stackers
    attachChild(m_globalStacker);
    m_globalStacker.stackBack(m_colorBox, Align::CENTER);
    m_globalStacker.stackBack(m_slidersStacker, Align::CENTER);
    m_slidersStacker.setPadding(0.f);

    // Sliders
    for (auto& slider : m_sliders) {
        m_slidersStacker.stackBack(slider);
        slider.setVisibleSteps(0u);
        slider.setRange(0u, 255u);
        slider.setValueChangedCallback([this] (uint value) { updateColorBox(); });
    }

    m_sliders[0u].setIndicatorTiltColor(sf::Color::Red);
    m_sliders[1u].setIndicatorTiltColor(sf::Color::Green);
    m_sliders[2u].setIndicatorTiltColor(sf::Color::Blue);

    updateColorBox();
}

//-------------------//
//----- Routine -----//

void ColorSelector::onChildSizeChanges(scene::Entity& child)
{
    returnif (&child != &m_globalStacker);

    float boxSide = child.size().y;
    m_colorBox.setSize({boxSide, boxSide});
    setSize(child.size());
}

//---------------//
//----- ICU -----//

void ColorSelector::updateColorBox()
{
    sf::Color color;

    color.r = m_sliders[0u].value();
    color.g = m_sliders[1u].value();
    color.b = m_sliders[2u].value();

    m_colorBox.setFillColor(color);
}
