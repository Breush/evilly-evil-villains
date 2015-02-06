#include "interaction/detectable.hpp"

#include "interaction/mousedetector.hpp"

using namespace interaction;

Detectable::Detectable()
    : m_detectable(true)
    , m_mouseDetector(nullptr)
{
}

Detectable::~Detectable()
{
}

void Detectable::detectableDraw(sf::RenderStates states) const
{
    if (m_mouseDetector != nullptr)
        m_mouseDetector->draw(*this, states);
}
