#include "sfe/gauge.hpp"

#include "config/nuiguides.hpp"
#include "tools/tools.hpp"
#include "tools/debug.hpp"
#include "tools/vector.hpp"
#include "tools/platform-fixes.hpp" // make_unique

using namespace sfe;

Gauge::Gauge()
    : m_length(100.f)
    , m_verticalOrientation(true)
    , m_invertedAppearance(false)
{
    addPart(&m_background);
    addPart(&m_filler);

    // Visual
    m_background.setOutlineColor(sf::Color::White);
    m_background.setOutlineThickness(1.f);
    m_background.setFillColor(sf::Color::Black);
    m_filler.setFillColor(sf::Color::White);
}

//-------------------//
//----- Routine -----//

void Gauge::onSizeChanges()
{
    m_background.setSize(size());
    refreshIndicators();
    refreshFiller();
}

void Gauge::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_minHintSize = cNUI.hintSize * 4.f;
    m_maxHintSize = cNUI.hintSize / 8.f;

    updateSize();
}

//-------------------------//
//----- Configuration -----//

void Gauge::setValue(float inValue)
{
    if (inValue >= m_maxLimit) m_percent = 1.f;
    else if (inValue <= m_minLimit) m_percent = 0.f;
    else m_percent = (inValue - m_minLimit) / (m_maxLimit - m_minLimit);

    refreshFiller();
}

void Gauge::setFillColor(const sf::Color& color)
{
    m_filler.setFillColor(color);
}

void Gauge::setLimits(float minLimit, float maxLimit)
{
    massert(m_minLimit <= m_maxLimit, "Gauge limits are not logical.");

    m_minLimit = minLimit;
    m_maxLimit = maxLimit;
}

void Gauge::setVerticalOrientation(bool inVerticalOrientation)
{
    returnif (inVerticalOrientation == m_verticalOrientation);
    m_verticalOrientation = inVerticalOrientation;

    // We change all indicators orientation.
    for (auto& indicator : m_indicators) {
        // It was horizontal before
        if (m_verticalOrientation) {
            removePart(indicator.vline.get());
            indicator.vline = nullptr;
            indicator.hline = std::make_unique<sfe::HLine>();
            addPart(indicator.hline.get());
        }

        // It was vertical before
        else {
            removePart(indicator.hline.get());
            indicator.hline = nullptr;
            indicator.vline = std::make_unique<sfe::VLine>();
            addPart(indicator.vline.get());
        }
    }

    updateSize();
}

void Gauge::addIndicator(float inValue, const sf::Color& color)
{
    Indicator indicator;
    indicator.value = inValue;
    indicator.color = color;

    // Vertical, implies horizontal indicator
    if (m_verticalOrientation) {
        indicator.hline = std::make_unique<sfe::HLine>();
        addPart(indicator.hline.get());
    }

    // Horizontal, implies horizontal indicator
    else {
        indicator.vline = std::make_unique<sfe::VLine>();
        addPart(indicator.vline.get());
    }

    m_indicators.emplace_back(std::move(indicator));

    refreshIndicators();
    refreshFiller();
}

//-------------------//
//----- Getters -----//

float Gauge::value() const
{
    return m_minLimit + m_percent * (m_maxLimit - m_minLimit);
}

//-------------------//
//----- Changes -----//

void Gauge::updateSize()
{
    auto minSideSize = m_minHintSize;
    auto maxSideSize = m_length * m_maxHintSize;

    if (m_verticalOrientation) setSize({minSideSize, maxSideSize});
    else setSize({maxSideSize, minSideSize});
}

void Gauge::refreshIndicators()
{
    for (auto& indicator : m_indicators) {
        float percent = 0.f;
        auto inValue = indicator.value;
        if (inValue >= m_maxLimit) percent = 1.f;
        else if (inValue > m_minLimit) percent = (inValue - m_minLimit) / (m_maxLimit - m_minLimit);

        // Vertical
        if (m_verticalOrientation) {
            indicator.hline->setShade(0.05f);
            indicator.hline->setLength(size().x);
            indicator.hline->setColor(indicator.color);
            if (m_invertedAppearance) indicator.hline->setPosition({0.f, percent * size().y});
            else indicator.hline->setPosition({0.f, (1.f - percent) * size().y});
        }

        // Horizontal
        else {
            indicator.vline->setShade(0.05f);
            indicator.vline->setLength(size().y);
            indicator.vline->setColor(indicator.color);
            if (!m_invertedAppearance) indicator.vline->setPosition({percent * size().x, 0.f});
            else indicator.vline->setPosition({(1.f - percent) * size().x, 0.f});
        }
    }
}

void Gauge::refreshFiller()
{
    // Vertical
    if (m_verticalOrientation) {
        m_filler.setSize({size().x, m_percent * size().y});

        // Inverse is top -> bottom
        if (m_invertedAppearance) m_filler.setPosition(0.f, 0.f);
        else m_filler.setPosition(0.f, (1.f - m_percent) * size().y);
    }
    // Horizontal
    else {
        m_filler.setSize({m_percent * size().x, size().y});

        // Inverse is right -> left
        if (m_invertedAppearance) m_filler.setPosition((1.f - m_percent) * size().x, 0.f);
        else m_filler.setPosition(0.f, 0.f);
    }
}
