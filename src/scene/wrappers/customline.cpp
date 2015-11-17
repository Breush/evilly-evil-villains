#include "scene/wrappers/customline.hpp"

#include "tools/math.hpp"
#include "tools/tools.hpp"
#include "tools/vector.hpp"

using namespace scene;

CustomLine::CustomLine()
{
    setDetectable(false);

    addPart(&m_curve);
}

//-------------------//
//----- Routine -----//

void CustomLine::updateRoutine(const sf::Time& dt)
{
    returnif (!visible());

    m_time += dt.asSeconds();
    m_tFactor = 0.1f * std::cos(m_time);
    refreshCurve();
}

//--------------------//
//----- Wrappers -----//

void CustomLine::setLimits(const sf::Vector2f& a, const sf::Vector2f& b)
{
    m_a = a;
    m_b = b;

    m_d = m_b - m_a;
    m_t = {-m_d.y, m_d.x};

    refreshCurve();
}

//---------------//
//----- ICU -----//

void CustomLine::refreshCurve()
{
    auto c1 = m_a + m_d * 0.3f + m_tFactor * m_t;
    auto c2 = m_b - m_d * 0.3f - m_tFactor * m_t;
    m_curve.set(m_a, m_b, c1, c2);
}
