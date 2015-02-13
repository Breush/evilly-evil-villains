#include "nui/stacker.hpp"

#include "tools/debug.hpp"

using namespace nui;

Stacker::Stacker()
    : baseClass()
    , m_margin(10)
    , m_align(Stacker::STANDARD)
{
    setDetectable(false);
}

//-------------------------------//
//----- Children management -----//

void Stacker::add(scene::Entity* child, Align inAlign)
{
    attachChild(*child);
    m_children.push_back({child, inAlign});
    update();
}
