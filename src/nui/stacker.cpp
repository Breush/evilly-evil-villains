#include "nui/stacker.hpp"

#include "tools/debug.hpp"

using namespace nui;

Stacker::Stacker()
    : baseClass()
    , m_margin(10)
    , m_align(Stacker::Align::STANDARD)
{
    setDetectable(false);
}

//-------------------------------//
//----- Children management -----//

void Stacker::add(scene::Entity* child, Align inAlign)
{
    attachChild(*child);
    m_children.emplace_back(ChildInfo{child, inAlign});
    update();
}

void Stacker::clear()
{
    for (auto& child : m_children)
        detachChild(*child.entity);
    m_children.clear();
}
