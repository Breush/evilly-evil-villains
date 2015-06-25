#include "dungeon/summary.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "config/nui.hpp"
#include "context/villains.hpp"
#include "dungeon/data.hpp"

using namespace dungeon;

Summary::Summary()
{
    const auto& font = Application::context().fonts.get(FontID::NUI);

    // Background
    m_background.setFillColor({0, 0, 0, 120});
    m_background.setOutlineColor({120, 120, 120, 120});
    m_background.setOutlineThickness(1.f);
    addPart(&m_background);

    // Dungeon name
    m_dungeonName.setFont(font);
    m_dungeonName.setColor(sf::Color::White);
    m_dungeonName.setStyle(sf::Text::Bold | sf::Text::Underlined);
    addPart(&m_dungeonName);

    // Bars
    // TODO Use images for logos
    m_bars[BAR_DOSH].logo.setFillColor(sf::Color::Yellow);
    m_bars[BAR_FAME].logo.setFillColor(sf::Color::Green);

    for (auto& bar : m_bars) {
        bar.text.setFont(font);
        bar.text.setColor(sf::Color::White);

        addPart(&bar.logo);
        addPart(&bar.text);
    }

    refreshDisplay();
}

//-------------------//
//----- Routine -----//

void Summary::onSizeChanges()
{
    m_background.setSize(size());

    const auto& nameBounds = m_dungeonName.getLocalBounds();
    m_dungeonName.setPosition((size().x - (nameBounds.left + nameBounds.width)) / 2.f, m_vPadding);
}

void Summary::updateSize()
{
    const auto& nameBounds = m_dungeonName.getLocalBounds();
    auto maxWidth = std::max(m_barWidth, 2.f * m_hPadding + nameBounds.left + nameBounds.width);

    setSize({maxWidth, m_vPadding + (BAR_COUNT + 1.f) * m_barHeight});
}

void Summary::refreshDisplay()
{
    config::NUI cNUI;

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
    baseClass::refreshDisplay();
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

void Summary::receive(const Event& event)
{
    if (event.type == EventType::DOSH_CHANGED
        || event.type == EventType::FAME_CHANGED)
        refreshFromData();
}

//-----------------------------------//
//----- Internal change updates -----//

void Summary::refreshFromData()
{
    m_dungeonName.setString(m_data->name());

    m_bars[BAR_DOSH].text.setString(toWString(m_data->villain().doshWallet.value()));
    m_bars[BAR_FAME].text.setString(toWString(m_data->fame()));

    updateSize();
}
