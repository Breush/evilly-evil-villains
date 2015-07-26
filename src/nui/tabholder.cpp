#include "nui/tabholder.hpp"

#include "core/gettext.hpp"
#include "resources/identifiers.hpp"
#include "dungeon/sidebar.hpp"
#include "sfe/lerpable.hpp"
#include "tools/platform-fixes.hpp" // make_unique
#include "tools/debug.hpp"
#include "tools/tools.hpp"

using namespace nui;

TabHolder::TabHolder()
    : m_width(0.f)
{
    // Tabs stacker
    attachChild(m_tabsStacker);
    m_tabsStacker.centerOrigin();
    m_tabsStacker.setRelativePosition({0.5f, 0.5f});
}

//-------------------//
//----- Routine -----//

void TabHolder::onChildSizeChanges(scene::Entity& child)
{
    m_height = child.size().y;
    refreshSize();
}

//------------------//
//----- Events -----//

void TabHolder::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f&, const sf::Vector2f& nuiPos)
{
    returnif (button != sf::Mouse::Left);

    uint tabNumber = 0u;
    for (auto& tab : m_tabs) {
        // Find tab bounds
        const auto& tabPosition = tab.image->getPosition();
        const auto& tabSize = tab.image->size();
        sf::FloatRect tabBounds{tabPosition.x, tabPosition.y, tabSize.x, tabSize.y};

        // If click happened over the tab, select it
        if (tabBounds.contains(nuiPos)) {
            select(tabNumber);
            break;
        }

        ++tabNumber;
    }
}

void TabHolder::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
}

void TabHolder::handleMouseLeft()
{
}

//---------------------------//
//----- Tabs management -----//

void TabHolder::stack_back(std::wstring tooltipString, TextureID textureID, const SelectCallback& callback)
{
    auto image = std::make_unique<sfe::RectangleShape>();
    image->setTexture(textureID);
    image->setSize({40.f, 40.f});

    m_tabs.push_back({std::move(image), std::move(tooltipString), callback});
    m_tabsStacker.stackBack(*m_tabs.back().image, nui::Align::CENTER);
}

void TabHolder::select(uint tabNumber)
{
    massert(tabNumber < m_tabs.size(), "Tab number " << tabNumber << " is too big.");

    const auto& selectedTab = m_tabs[tabNumber];
    if (selectedTab.callback != nullptr)
        selectedTab.callback();
}

//-----------------------------------//
//----- Internal change updates -----//

void TabHolder::refreshSize()
{
    setSize({m_width, m_height});
}
