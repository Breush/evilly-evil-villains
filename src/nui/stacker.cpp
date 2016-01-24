#include "nui/stacker.hpp"

#include "context/context.hpp"
#include "tools/debug.hpp"
#include "tools/platform-fixes.hpp" // erase_if

using namespace nui;

Stacker::Stacker()
{
    setDetectable(false);
}

//-------------------//
//----- Routine -----//

void Stacker::onChildDetached(scene::Entity& child)
{
    std::erase_if(m_children, [&](const Child& inChild) { return inChild.entity == &child; });
    updateSize();
}

//-------------------//
//----- Display -----//

void Stacker::setPadding(const float inPadding)
{
    m_paddingOverwritten = (inPadding >= 0.f);

    if (m_paddingOverwritten)
        m_padding = inPadding;
    else
        refreshNUI(context::context.nuiGuides);
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
    // We need a copy as m_children will be modified
    auto childrenList(m_children);
    for (auto& child : childrenList)
        detachChild(*child.entity);
}
