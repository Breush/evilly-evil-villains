#include "nui/slider.hpp"

#include "core/application.hpp"
#include "config/nuiguides.hpp"
#include "tools/string.hpp"
#include "tools/tools.hpp"

using namespace nui;

Slider::Slider()
    : m_length(10.f)
{
    setFocusable(true);

    // Bars
    const auto& barLeftTexture = Application::context().textures.get("nui/slider/bar_left");
    const auto& barMiddleTexture = Application::context().textures.get("nui/slider/bar_middle");
    const auto& barRightTexture = Application::context().textures.get("nui/slider/bar_right");

    m_barLeft.setTexture(&barLeftTexture);
    m_barMiddle.setTexture(&barMiddleTexture);
    m_barRight.setTexture(&barRightTexture);

    m_barLeftWidth = barLeftTexture.getSize().x;
    m_barRightWidth = barRightTexture.getSize().x;

    m_barRight.setOrigin(m_barRightWidth, 0.f);

    // Traces
    m_traceTexture = &Application::context().textures.get("nui/slider/trace");
    m_traceWidth = m_traceTexture->getSize().x;

    // Indicator
    const auto& indicatorTexture = Application::context().textures.get("nui/slider/indicator");
    m_indicator.setTexture(&indicatorTexture);
    m_indicatorWidth = indicatorTexture.getSize().x;
    m_indicator.setFillColor(sf::Color::Green);
}

//-------------------//
//----- Routine -----//

void Slider::onSizeChanges()
{
    refreshElements();
}

void Slider::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_widthHint = 1.f + cNUI.hintSize;
    m_heightHint = cNUI.hintSize * 2.f;
    m_fontSize = std::floor(0.9f * cNUI.fontSize);
    m_barOffset = std::floor(m_fontSize);

    updateSize();
}

void Slider::updateSize()
{
    float realOffset = (m_visibleSteps == 0u)? 0.f : m_barOffset;
    setSize({m_length * m_widthHint, m_heightHint + realOffset});
}

//------------------//
//----- Events -----//

void Slider::handleGlobalMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left);

    returnif (!m_grabbing);
    m_grabbing = false;

    setPartShader(&m_indicator, "");
}

void Slider::handleGlobalMouseMoved(const sf::Vector2f& nuiPos)
{
    returnif (!m_grabbing);

    auto mousePos = getInverseTransform().transformPoint(nuiPos);
    setClosestValue(mousePos.x);
}

bool Slider::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left) false;
    setClosestValue(mousePos.x);
    m_grabbing = true;
    return true;
}

bool Slider::handleMouseMoved(const sf::Vector2f&, const sf::Vector2f&)
{
    setPartShader(&m_indicator, "nui/hover");
    return true;
}

void Slider::handleMouseLeft()
{
    returnif (m_grabbing);
    setPartShader(&m_indicator, "");
}

bool Slider::handleKeyboardEvent(const sf::Event& event)
{
    returnif (event.type != sf::Event::KeyPressed) false;

    if (event.key.code == sf::Keyboard::Left) {
        if (m_value != m_min) setValue(m_value - 1u);
        return true;
    }
    else if (event.key.code == sf::Keyboard::Right) {
        if (m_value != m_max) setValue(m_value + 1u);
        return true;
    }

    return false;
}

//-------------------------//
//----- Configuration -----//

void Slider::setClosestValue(const float relX)
{
    const float distance = size().x / (m_max - m_min);
    auto closestValue = std::round(relX / distance);

    uint value = m_min;
    if (closestValue >= 0.f)
        value += static_cast<uint>(closestValue);

    if (value != m_value)
        setValue(value);
}

void Slider::setValue(uint inValue)
{
    if (inValue >= m_max) inValue = m_max;
    else if (inValue <= m_min) inValue = m_min;

    returnif (m_value == inValue);

    m_value = inValue;
    if (m_valueChangedCallback)
        m_valueChangedCallback(m_value);

    refreshIndicator();
}

void Slider::setRange(uint min, float max)
{
    m_min = min;
    m_max = max;
    setValue(value());
    refreshElements();
}

void Slider::setVisibleSteps(uint visibleSteps)
{
    m_visibleSteps = visibleSteps;
    updateSize();
}

void Slider::setIndicatorTiltColor(const sf::Color& tiltColor)
{
    m_indicator.setFillColor(tiltColor);
}

//---------------------//
//----- Callbacks -----//

void Slider::setValueChangedCallback(ValueChangedCallback valueChangedCallback)
{
    m_valueChangedCallback = valueChangedCallback;
}

//---------------//
//----- ICU -----//

void Slider::refreshElements()
{
    // Remove all parts
    clearParts();

    refreshBar();
    refreshTraces();
    refreshIndicator();

    // Now adding them all
    addPart(&m_barLeft);
    addPart(&m_barMiddle);
    addPart(&m_barRight);

    for (auto& trace : m_traces) {
        addPart(&trace.line);
        addPart(&trace.text);
    }

    addPart(&m_indicator);
}

void Slider::refreshBar()
{
    float realOffset = (m_visibleSteps == 0u)? 0.f : m_barOffset;
    float barHeight = size().y - realOffset;

    m_barLeft.setPosition(0.f, realOffset);
    m_barMiddle.setPosition(m_barLeftWidth, realOffset);
    m_barRight.setPosition(size().x, realOffset);

    m_barLeft.setSize({m_barLeftWidth, barHeight});
    m_barMiddle.setSize({size().x - (m_barLeftWidth + m_barRightWidth), barHeight});
    m_barRight.setSize({m_barRightWidth, barHeight});
}

void Slider::refreshTraces()
{
    returnif (m_visibleSteps == 0u);

    float offset = 0.f;
    float barHeight = size().y - m_barOffset;
    uint traceValue = m_min;

    uint steps = m_max - m_min;
    if (m_visibleSteps < steps)
        steps = m_visibleSteps;

    returnif (steps == 0u);

    const float stepDistance = size().x / steps;
    const uint traceStep = (m_max - m_min + 1u) / steps;

    m_traces.resize(steps + 1u);
    for (auto& trace : m_traces) {
        trace.line.setTexture(m_traceTexture);
        trace.line.setSize({m_traceWidth, barHeight});
        trace.line.setPosition({offset, m_barOffset});
        trace.line.setOrigin({0.5f * m_traceWidth, 0.f});

        trace.text.setFont(Application::context().fonts.get("nui"));
        trace.text.setCharacterSize(m_fontSize);
        trace.text.setString(toString(traceValue));

        auto textSize = boundsSize(trace.text);
        trace.text.setPosition({offset, 0.f});
        trace.text.setOrigin({0.5f * textSize.x, 0.f});

        offset += stepDistance;
        traceValue += traceStep;
    }
}

void Slider::refreshIndicator()
{
    const float distance = size().x / (m_max - m_min);
    const float position = (m_value - m_min) * distance;

    float realOffset = (m_visibleSteps == 0u)? 0.f : m_barOffset;
    float barHeight = size().y - realOffset;

    m_indicator.setSize({m_indicatorWidth, barHeight});
    m_indicator.setPosition({position, realOffset});
    m_indicator.setOrigin({0.5f * m_indicatorWidth, 0.f});
}
