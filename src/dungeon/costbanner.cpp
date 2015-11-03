#include "dungeon/costbanner.hpp"

#include "core/application.hpp"
#include "tools/string.hpp"
#include "tools/tools.hpp"

using namespace dungeon;

CostBanner::CostBanner()
{
    // Text
    m_text.setFont(Application::context().fonts.get("nui"));

    // Icons
    m_icons[IconID::DOSH].setTexture(&Application::context().textures.get("elements/dosh"));
    m_icons[IconID::SOUL].setTexture(&Application::context().textures.get("elements/soul"));
    m_icons[IconID::FAME].setTexture(&Application::context().textures.get("elements/fame"));
}

//-------------------//
//----- Routine -----//

void CostBanner::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_refSize = 0.7f * cNUI.fontSize;

    refreshBanner();
}

//-------------------//
//----- Control -----//

void CostBanner::setCost(const Cost& cost)
{
    m_cost = cost;
    refreshBanner();
}

//-----------------------------------//
//----- Internal changes update -----//

void CostBanner::addString(std::wstring& str, bool& first, uint value, sf::RectangleShape& icon)
{
    returnif (value == 0u);

    if (!first) str += L"     ";
    str += toWString(value);
    m_text.setString(str);
    icon.setPosition(boundsSize(m_text).x + 0.1f * m_refSize, 0.f);
    addPart(&icon);
    first = false;
}

void CostBanner::refreshBanner()
{
    clearParts();
    addPart(&m_text);

    // Size of elements
    m_text.setCharacterSize(m_refSize);
    for (auto& icon : m_icons)
        icon.setSize({m_refSize, m_refSize});

    // Texts
    std::wstring str;
    bool first = true;
    addString(str, first, m_cost.dosh, m_icons[IconID::DOSH]);
    addString(str, first, m_cost.soul, m_icons[IconID::SOUL]);
    addString(str, first, m_cost.fame, m_icons[IconID::FAME]);

    auto textSize = boundsSize(m_text);
    setSize({textSize.x + 1.1f * m_refSize, textSize.y});
}
