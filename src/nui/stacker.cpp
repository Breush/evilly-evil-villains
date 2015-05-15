#include "nui/stacker.hpp"

#include "tools/debug.hpp"

using namespace nui;

Stacker::Stacker()
{
    setDetectable(false);
}

//-------------------------------//
//----- Children management -----//

void Stacker::stackBack(scene::Entity& entity, Align inAlign)
{
    attachChild(entity);
    m_children.push_back({&entity, inAlign});
    updateSize();
}

void Stacker::unstackAll()
{
    for (auto& child : m_children)
        detachChild(*child.entity);
    m_children.clear();
    updateSize();
}
