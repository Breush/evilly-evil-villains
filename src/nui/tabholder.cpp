#include "nui/tabholder.hpp"

#include "core/gettext.hpp"
#include "dungeon/sidebar.hpp"
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
    m_background.setFillColor({255u, 255u, 255u, 25u});

    refreshContent();
}

//-------------------//
//----- Routine -----//

void TabHolder::onSizeChanges()
{
    // Background
    // The -2.f are for the outline of the background.
    m_background.setSize({size().x - 2.f, size().y - headerSize().y - 1.f});
    m_background.setPosition({0.f, headerSize().y - 1.f});
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

void TabHolder::stackBack(std::wstring tooltipString, const std::string& textureID, scene::Entity& content)
{
    auto image = std::make_unique<scene::RectangleShape>();
    image->setTexture(textureID);
    image->setSize({40.f, 40.f});

    m_tabs.push_back({std::move(image), std::move(tooltipString), content});
    m_tabsStacker.stackBack(*m_tabs.back().image, nui::Align::CENTER);

    refreshTabBackground();

    // Select first tab on add
    if (m_tabs.size() == 1u)
        select(0u);
}

void TabHolder::select(uint tabNumber)
{
    massert(tabNumber < m_tabs.size(), "Tab number " << tabNumber << " is too big.");
    m_selectedTab = tabNumber;
    refreshSelectedTab();
    refreshContent();
}

//-------------------------------//
//----- Setters and getters -----//

sf::Vector2f TabHolder::headerSize() const
{
    sf::Vector2f oHeaderSize;

    // Width
    oHeaderSize.x = size().x;

    // Height
    oHeaderSize.y = 0.f;
    for (auto& tab : m_tabs)
        oHeaderSize.y = std::max(oHeaderSize.y, tab.image->size().y);

    return oHeaderSize;
}

//-----------------------------------//
//----- Internal change updates -----//

void TabHolder::refreshSelectedTab()
{
    for (uint tabNumber = 0u; tabNumber < m_tabs.size(); ++tabNumber) {
        auto& tabBackground = m_tabsBackgrounds[tabNumber];
        if (m_selectedTab == tabNumber) tabBackground.setFillColor(sf::Color::White);
        else                            tabBackground.setFillColor(sf::Color::Black);
    }
}

void TabHolder::refreshTabBackground()
{
    // Remove all previous
    for (auto& tabBackground : m_tabsBackgrounds)
        removePart(&tabBackground);
    m_tabsBackgrounds.clear();
    m_tabsBackgrounds.resize(m_tabs.size());

    // Add all and position
    float headerHeight = headerSize().y;
    for (uint tabNumber = 0u; tabNumber < m_tabs.size(); ++tabNumber) {
        const auto& tab = m_tabs[tabNumber];
        auto& tabBackground = m_tabsBackgrounds[tabNumber];
        tabBackground.setOutlineColor(sf::Color::White);
        tabBackground.setOutlineThickness(1.f);
        tabBackground.setSize({tab.image->size().x - 2.f, headerHeight - 2.f});
        tabBackground.setPosition(tab.image->localPosition());
        addPart(&tabBackground);
    }

    refreshSelectedTab();
}

void TabHolder::refreshContent()
{
    m_globalStacker.unstackAll();
    m_globalStacker.stackBack(m_tabsStacker);

    if (m_selectedTab < m_tabs.size())
        m_globalStacker.stackBack(m_tabs[m_selectedTab].content, nui::Align::CENTER);
}
