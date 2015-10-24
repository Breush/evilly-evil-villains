#include "nui/rangeselector.hpp"

#include "core/application.hpp"
#include "config/nuiguides.hpp"
#include "tools/string.hpp"
#include "tools/tools.hpp"

using namespace nui;

RangeSelector::RangeSelector()
    : m_length(100.f)
{
    m_mainLine.setFillColor(sf::Color::White);
    m_indicator.setFillColor({174u, 198u, 207u, 255u});
}

//-------------------//
//----- Routine -----//

void RangeSelector::onSizeChanges()
{
    refreshElements();
}

void RangeSelector::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_widthHint = cNUI.hintSize / 10.f;
    m_heightHint = cNUI.hintSize * 3.f;
    m_fontSize = std::floor(0.9f * cNUI.fontSize);

    updateSize();
}

//------------------//
//----- Events -----//

bool RangeSelector::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left) false;
    setClosestValue(mousePos.x);
    m_grabbing = true;
    return true;
}

bool RangeSelector::handleMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left) false;
    m_grabbing = false;
    return true;
}

bool RangeSelector::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    returnif (!m_grabbing) false;
    setClosestValue(mousePos.x);
    return true;
}

void RangeSelector::handleMouseLeft()
{
    m_grabbing = false;
}

//-------------------------//
//----- Configuration -----//

void RangeSelector::setClosestValue(const float relX)
{
    const float distance = size().x / (m_max - m_min);
    auto closestValue = std::round(relX / distance);
    setValue(static_cast<uint>(closestValue));
}

void RangeSelector::setValue(uint inValue)
{
    if (inValue >= m_max) m_value = m_max;
    else if (inValue <= m_min) m_value = m_min;
    else m_value = inValue;
    refreshElements();
}

void RangeSelector::setRange(uint min, float max)
{
    m_min = min;
    m_max = max;
    setValue(value());
    refreshElements();
}

//-------------------//
//----- Changes -----//

void RangeSelector::updateSize()
{
    setSize({m_length * m_widthHint, m_heightHint});
}

void RangeSelector::refreshIndicator()
{
    const float distance = size().x / (m_max - m_min);
    const float position = (m_value - m_min) * distance;

    // The indicator
    addPart(&m_indicator);
    m_indicator.setSize({4.f, 0.5f * size().y});
    m_indicator.setPosition({position, 0.75f * size().y});
    m_indicator.setOrigin({2.f, 0.25f * size().y});
}

void RangeSelector::refreshElements()
{
    const float distance = size().x / (m_max - m_min);

    clearParts();

    // Main line
    addPart(&m_mainLine);
    m_mainLine.setSize({size().x, 2.f});
    m_mainLine.setPosition({0.f, 0.75f * size().y});
    m_mainLine.setOrigin({0.f, 1.f});

    // All the vertical lines and their text
    float offset = 0.f;
    uint traceValue = m_min;

    m_traces.resize(m_max - m_min + 1u);
    for (auto& trace : m_traces) {
        addPart(&trace.line);
        trace.line.setSize({2.f, 0.25f * size().y});
        trace.line.setPosition({offset, 0.75f * size().y});
        trace.line.setOrigin({1.f, 0.125f * size().y});

        addPart(&trace.text);
        trace.text.setFont(Application::context().fonts.get("nui"));
        trace.text.setCharacterSize(m_fontSize);
        trace.text.setString(toString(traceValue));

        auto textSize = boundsSize(trace.text);
        trace.text.setPosition({offset, 0.f});
        trace.text.setOrigin({0.5f * textSize.x, 0.f});

        offset += distance;
        traceValue += 1u;
    }

    refreshIndicator();
}
