#include "dungeon/elements/dynamicelement.hpp"

using namespace dungeon;

DynamicElement::DynamicElement(Inter& inter)
    : baseClass(inter)
{
}

//---------------------------//
//----- LUA interaction -----//

uint DynamicElement::lua_getCurrentRoomX() const
{
    return static_cast<uint>(m_edata->operator[](L"rx").as_float());
}

uint DynamicElement::lua_getCurrentRoomY() const
{
    return static_cast<uint>(m_edata->operator[](L"ry").as_float());
}
