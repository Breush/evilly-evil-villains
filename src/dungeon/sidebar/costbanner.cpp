#include "dungeon/sidebar/costbanner.hpp"

#include "core/application.hpp"
#include "tools/string.hpp"
#include "tools/tools.hpp"

using namespace dungeon;

CostBanner::CostBanner()
{
    // Text
    m_text.setFont(Application::context().fonts.get("mono"));

    // Icons
    m_icons[IconID::DOSH].setTexture(&Application::context().textures.get("resources/dosh"));
    m_icons[IconID::SOUL].setTexture(&Application::context().textures.get("resources/soul"));
    m_icons[IconID::FAME].setTexture(&Application::context().textures.get("resources/fame"));
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

void CostBanner::setTitle(std::wstring title)
{
    m_title = std::move(title);
    refreshBanner();
}

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

    if (!first) str += L"\n";
    str += L"  " + toWString(value);
    m_text.setString(str);
    first = false;

    icon.setPosition(0.f, boundsSize(m_text).y);
    addPart(&icon);
}

void CostBanner::refreshBanner()
{
    clearParts();
    addPart(&m_text);

    // Icons
    float iconRefSize = 0.8f * m_refSize;
    for (auto& icon : m_icons) {
        icon.setSize({iconRefSize, iconRefSize});
        icon.setOrigin(0.f, iconRefSize);
    }

    // Texts
    bool first = true;
    std::wstring str = m_title.empty()? L"" : L"_" + m_title + L"_\n";
    m_text.setCharacterSize(m_refSize);
    addString(str, first, m_cost.dosh, m_icons[IconID::DOSH]);
    addString(str, first, m_cost.soul, m_icons[IconID::SOUL]);
    addString(str, first, m_cost.fame, m_icons[IconID::FAME]);

    auto textSize = boundsSize(m_text);
    setSize({textSize.x + 1.1f * m_refSize, textSize.y});
}
