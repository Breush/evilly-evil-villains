#include "nui/tabholder.hpp"

#include "core/gettext.hpp"
#include "dungeon/sidebar.hpp"
#include "config/nuiguides.hpp"
#include "tools/platform-fixes.hpp" // make_unique
#include "tools/debug.hpp"
#include "tools/tools.hpp"

using namespace nui;

TabHolder::TabHolder()
{
    attachChild(m_globalStacker);

    // Decorum
    m_background.setOutlineColor(sf::Color::White);
    m_background.setFillColor({255u, 255u, 255u, 25u});

    refreshContent();
}

//-------------------//
//----- Routine -----//

void TabHolder::onSizeChanges()
{
    refreshBackground();
}

void TabHolder::onChildSizeChanges(scene::Entity& child)
{
    updateSize();
}

void TabHolder::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_borderThick = cNUI.borderThick;
    m_vPadding = cNUI.vPadding;
    m_tabSpacing = std::floor(cNUI.hPadding / 2.f);
    m_tabSize = std::floor(cNUI.hintImageSide * 0.75f);

    // Refresh content position
    m_globalStacker.setLocalPosition({m_borderThick, m_tabSize});

    refreshBackground();
    refreshTabs();
}

//------------------//
//----- Events -----//

bool TabHolder::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left) false;

    uint tabNumber = 0u;
    for (auto& tab : m_tabs) {
        // Find tab bounds
        const auto& tabPosition = tab.image->localPosition();
        const auto& tabSize = tab.image->size();
        sf::FloatRect tabBounds{tabPosition.x, tabPosition.y, tabSize.x, tabSize.y};

        // If click happened over the tab, select it
        if (tabBounds.contains(mousePos)) {
            select(tabNumber);
            break;
        }

        ++tabNumber;
    }

    return true;
}

bool TabHolder::handleMouseMoved(const sf::Vector2f&, const sf::Vector2f&)
{
    return true;
}

void TabHolder::handleMouseLeft()
{
}

//---------------------------//
//----- Tabs management -----//

void TabHolder::stackBack(std::wstring tooltipString, const std::string& textureID, scene::Entity& content)
{
    auto image = std::make_unique<nui::RectangleShape>();
    image->setTooltip(std::move(tooltipString));
    image->setTexture(textureID);
    attachChild(*image);

    // Tab background
    auto background = std::make_unique<sf::RectangleShape>();
    background->setOutlineColor(sf::Color::White);

    m_tabs.push_back({std::move(image), std::move(background), content});
    refreshTabs();

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
    sf::Vector2f headerSize;

    float tabsWidth = (m_tabSize + m_tabSpacing) * m_tabs.size() - m_tabSpacing;
    headerSize.x = std::max(m_globalStacker.size().x, tabsWidth);
    headerSize.y = m_tabSize;

    return headerSize;
}

//-----------------------------------//
//----- Internal change updates -----//

void TabHolder::refreshBackground()
{
    m_background.setSize({size().x - 2.f * m_borderThick, m_globalStacker.size().y});
    m_background.setPosition({m_borderThick, m_tabSize});
    m_background.setOutlineThickness(m_borderThick);
}

void TabHolder::refreshSelectedTab()
{
    for (uint tabNumber = 0u; tabNumber < m_tabs.size(); ++tabNumber) {
        auto& tabBackground = *m_tabs.at(tabNumber).background;
        if (m_selectedTab == tabNumber) tabBackground.setFillColor(sf::Color::White);
        else                            tabBackground.setFillColor({255u, 255u, 255u, 84u});
    }
}

void TabHolder::refreshTabs()
{
    float imageSide = m_tabSize - 2.f * m_borderThick;

    // Remove all previous
    clearParts();
    addPart(&m_background);

    // Reparameter all and position them
    sf::Vector2f tabPosition(m_borderThick, m_borderThick);
    for (uint tabNumber = 0u; tabNumber < m_tabs.size(); ++tabNumber) {
        const auto& tab = m_tabs.at(tabNumber);

        // Background
        auto& tabBackground = *tab.background;
        tabBackground.setOutlineThickness(m_borderThick);
        tabBackground.setSize({imageSide, imageSide});
        tabBackground.setPosition(tabPosition);
        addPart(&tabBackground);

        // Image
        auto& tabImage = *tab.image;
        tabImage.setSize({imageSide, imageSide});
        tabImage.setLocalPosition(tabPosition);

        tabPosition.x += m_tabSize + m_tabSpacing;
    }

    refreshSelectedTab();
    updateSize();
}

void TabHolder::refreshContent()
{
    m_globalStacker.unstackAll();

    if (m_selectedTab < m_tabs.size())
        m_globalStacker.stackBack(m_tabs.at(m_selectedTab).content, nui::Align::CENTER);

    updateSize();
}

void TabHolder::updateSize()
{
    auto newSize = headerSize();
    newSize.y += m_globalStacker.size().y + m_borderThick;
    setSize(newSize);
}
