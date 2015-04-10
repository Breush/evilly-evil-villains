#include "sfe/gauge.hpp"

#include "tools/debug.hpp"
#include "config/nui.hpp"

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

    refreshDisplay();
}

//-------------------//
//----- Routine -----//

void Gauge::update()
{
    m_background.setSize(size());
    refreshFiller();
}

void Gauge::refreshDisplay()
{
    config::NUI cNUI;

    m_minHintSize = cNUI.hintSize * 4.f;
    m_maxHintSize = cNUI.hintSize / 8.f;

    updateSize();
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

//-------------------//
//----- Getters -----//

float Gauge::value()
{
    return m_minLimit + m_percent * (m_maxLimit - m_minLimit);
}
