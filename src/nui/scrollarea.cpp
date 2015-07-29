#include "nui/scrollarea.hpp"

#include "tools/tools.hpp"

using namespace nui;

ScrollArea::ScrollArea()
{
    setDetectable(false);
}

//-------------------//
//----- Routine -----//

void ScrollArea::onSizeChanges()
{
    returnif (m_content == nullptr);
    refreshContentStatus();
    refreshBars();
}

void ScrollArea::onChildSizeChanges(scene::Entity&)
{
    refreshContentStatus();
    refreshBars();
}

//------------------------------//
//----- Content management -----//

void ScrollArea::setContent(scene::Entity& entity)
{
    m_content = &entity;
    attachChild(*m_content);
    refreshContentStatus();
    refreshBars();
}

//-----------------------------------//
//----- Internal changes update -----//

void ScrollArea::refreshBars()
{
    returnif (m_content == nullptr);

    // TODO Recompute bar visibility + length
}

void ScrollArea::refreshContentStatus()
{
    returnif (m_content == nullptr);
    m_content->setLocalPosition(-m_offset);
    m_content->setClipArea({m_offset.x, m_offset.y, size().x, size().y});
}
