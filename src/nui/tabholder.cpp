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
{
    // General
    attachChild(m_globalStacker);

    // Decorum
    addPart(&m_background);
    m_background.setOutlineThickness(1.f);
    m_background.setOutlineColor(sf::Color::White);
    m_background.setFillColor({255u, 255u, 255u, 100u});

    refreshContent();
}

//-------------------//
//----- Routine -----//

void TabHolder::onSizeChanges()
{
    m_background.setSize(size());
}

void TabHolder::onChildSizeChanges(scene::Entity& child)
{
    // Whenever the global stacker size changes, we update ours
    setSize(child.size());
}

//------------------//
//----- Events -----//

void TabHolder::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left);

    uint tabNumber = 0u;
    for (auto& tab : m_tabs) {
        // Find tab bounds
        const auto& tabPosition = m_tabsStacker.localPosition() + tab.image->localPosition();
        const auto& tabSize = tab.image->size();
        sf::FloatRect tabBounds{tabPosition.x, tabPosition.y, tabSize.x, tabSize.y};

        // If click happened over the tab, select it
        if (tabBounds.contains(mousePos)) {
            select(tabNumber);
            break;
        }

        ++tabNumber;
    }
}

void TabHolder::handleMouseMoved(const sf::Vector2f&, const sf::Vector2f&)
{
}

void TabHolder::handleMouseLeft()
{
}

//---------------------------//
//----- Tabs management -----//

void TabHolder::stackBack(std::wstring tooltipString, TextureID textureID, scene::Entity& content)
{
    auto image = std::make_unique<sfe::RectangleShape>();
    image->setTexture(textureID);
    image->setSize({40.f, 40.f});

    m_tabs.push_back({std::move(image), std::move(tooltipString), content});
    m_tabsStacker.stackBack(*m_tabs.back().image, nui::Align::CENTER);

    // Select first tab on add
    if (m_tabs.size() == 1u) {
        m_selectedTab = &m_tabs.back();
        refreshContent();
    }
}

void TabHolder::select(uint tabNumber)
{
    massert(tabNumber < m_tabs.size(), "Tab number " << tabNumber << " is too big.");
    m_selectedTab = &m_tabs[tabNumber];
    refreshContent();
}

//-----------------------------------//
//----- Internal change updates -----//

void TabHolder::refreshContent()
{
    m_globalStacker.unstackAll();
    m_globalStacker.stackBack(m_tabsStacker);

    if (m_selectedTab != nullptr)
        m_globalStacker.stackBack(m_selectedTab->content, nui::Align::CENTER);
}
