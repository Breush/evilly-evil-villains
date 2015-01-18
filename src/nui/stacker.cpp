#include "nui/stacker.hpp"

#include "nui/uicore.hpp"
#include "tools/debug.hpp"

using namespace nui;

Stacker::Stacker()
    : baseClass()
    , m_margin(10)
    , m_align(Stacker::STANDARD)
{
    setFocusable(false);
    setDetectable(false);
}

//-------------------------------//
//----- Children management -----//

void Stacker::add(Object* child, Align inAlign)
{
    massert(core() != nullptr, "Stacker not affected to a core before adding child");
    child->setParent(this);
    core()->add(child);

    m_children.push_back({child, inAlign});
    update();
}

void Stacker::add(Object* child)
{
    add(child, Align::STANDARD);
}

void Stacker::changedChild(Object*)
{
    update();
}
