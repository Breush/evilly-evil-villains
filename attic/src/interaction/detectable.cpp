#include "interaction/detectable.hpp"

#include "interaction/mousedetector.hpp"

using namespace interaction;

Detectable::Detectable()
    : m_detectable(true)
    , m_zDepth(50)
{
}

Detectable::~Detectable()
{
}
