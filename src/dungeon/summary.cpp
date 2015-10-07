#include "dungeon/summary.hpp"

#include "core/application.hpp"
#include "context/villains.hpp"
#include "dungeon/data.hpp"
#include "tools/string.hpp"

using namespace dungeon;

Summary::Summary()
{
    const auto& font = Application::context().fonts.get("nui");

    // Dungeon name
    addPart(&m_dungeonName);
    m_dungeonName.setFont(font);
    m_dungeonName.setColor(sf::Color::White);
    m_dungeonName.setStyle(sf::Text::Bold | sf::Text::Underlined);

    // Bars
    for (auto& bar : m_bars) {
        addPart(&bar.logo);
        addPart(&bar.text);
        bar.text.setFont(font);
    }

    m_bars[BAR_DOSH].text.setColor({190u, 171u, 21u});
    m_bars[BAR_FAME].text.setColor({102u, 151u, 196u});
}

void Summary::init()
{
    // Bars
    m_bars[BAR_DOSH].logo.setTexture(&Application::context().textures.get("elements/dosh"));
    m_bars[BAR_FAME].logo.setTexture(&Application::context().textures.get("elements/fame"));
}

//-------------------//
//----- Routine -----//

void Summary::onSizeChanges()
{
    m_dungeonName.setPosition(size().x / 2.f, m_vPadding);
}

void Summary::updateSize()
{
    auto maxWidth = std::max(m_barWidth, 2.f * m_hPadding + boundsSize(m_dungeonName).x);
    setSize({maxWidth, m_vPadding + (BAR_COUNT + 1.f) * m_barHeight});
}

void Summary::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_barImageSide = cNUI.fontVSpace;
    m_fontSize = cNUI.fontSize;
    m_hPadding = cNUI.hPadding;
    m_vPadding = cNUI.vPadding;
    m_barWidth = 3.f * m_hPadding + m_barImageSide + 10.f * cNUI.fontHSpace;
    m_barHeight = m_vPadding + m_barImageSide;

    // Dungeon name
    m_dungeonName.setCharacterSize(m_fontSize);

    // Dungeon bars
    auto x = m_hPadding;
    auto y = m_vPadding;

    for (auto& bar : m_bars) {
        y += m_barHeight;

        bar.logo.setSize({m_barImageSide, m_barImageSide});
        bar.logo.setPosition(x, y);

        bar.text.setCharacterSize(m_fontSize);
        bar.text.setPosition(x + m_barImageSide + m_hPadding, y);
    }

    updateSize();
}

//------------------------//
//----- Dungeon data -----//

void Summary::useData(Data& data)
{
    m_data = &data;
    setEmitter(&data);
    refreshFromData();
}

//--------------------------//
//----- Dungeon events -----//

void Summary::receive(const context::Event& event)
{
    const auto& devent = *reinterpret_cast<const dungeon::Event*>(&event);
    if (devent.type == "dosh_changed" || devent.type == "fame_changed")
        refreshFromData();
}

//-----------------------------------//
//----- Internal change updates -----//

void Summary::refreshFromData()
{
    m_dungeonName.setString(m_data->name());
    m_dungeonName.setOrigin(boundsSize(m_dungeonName).x / 2.f, 0.f);

    m_bars[BAR_DOSH].text.setString(toWString(m_data->villain().doshWallet.value()));
    m_bars[BAR_FAME].text.setString(toWString(m_data->fame()));

    updateSize();
}
