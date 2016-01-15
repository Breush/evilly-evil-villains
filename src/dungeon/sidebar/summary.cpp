#include "dungeon/sidebar/summary.hpp"

#include "core/gettext.hpp"
#include "context/villains.hpp"
#include "context/worlds.hpp"
#include "dungeon/data.hpp"
#include "tools/string.hpp"

#include <iomanip>

using namespace dungeon;

Summary::Summary()
{
    // Frame
    attachChild(m_frame);
    m_frame.setContent(m_stacker);
    m_frame.setContentSizeChangedCallback([this] { updateSize(); });
    m_stacker.setPadding(0.f);

    // Bars
    for (auto& bar : m_bars) {
        bar = std::make_unique<SummaryBar>();
        m_stacker.stackBack(*bar);
    }
}

void Summary::init()
{
    // Bars
    for (auto& bar : m_bars)
        bar->init();

    m_bars[BAR_TIME]->setLogo("resources/time");
    m_bars[BAR_DOSH]->setLogo("resources/dosh");
    m_bars[BAR_SOUL]->setLogo("resources/soul");
    m_bars[BAR_FAME]->setLogo("resources/fame");

    m_bars[BAR_TIME]->setColor(sf::Color::White);
    m_bars[BAR_DOSH]->setColor({190u, 171u, 21u});
    m_bars[BAR_SOUL]->setColor({102u, 190u, 21u});
    m_bars[BAR_FAME]->setColor({102u, 151u, 196u});
}

//-------------------//
//----- Routine -----//

void Summary::onSizeChanges()
{
    refresh();
}

void Summary::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    // Translations
    m_bars[BAR_TIME]->setTooltip(_("Date and Time"));
    m_bars[BAR_DOSH]->setTooltip(_("Dosh"));
    m_bars[BAR_SOUL]->setTooltip(_("Souls"));
    m_bars[BAR_FAME]->setTooltip(_("Fame"));

    updateSize();
}

void Summary::updateSize()
{
    setSize({m_width, m_stacker.size().y + 2.f * m_frame.paddings().y});
}

//------------------//
//----- Events -----//

void Summary::receive(const context::Event& event)
{
    const auto& devent = *reinterpret_cast<const dungeon::Event*>(&event);

    if (devent.type == "time_changed")
        refreshTimeBar();
    else if (devent.type == "dosh_changed")
        refreshDoshBar();
    else if (devent.type == "soul_changed")
        refreshSoulBar();
    else if (devent.type == "fame_changed")
        refreshFameBar();
}

//------------------------//
//----- Dungeon data -----//

void Summary::useData(Data& data)
{
    m_data = &data;
    setEmitter(&data);
    refreshFromData();
}

//-------------------//
//----- Control -----//

void Summary::setWidth(float width)
{
    m_width = width;
    updateSize();
}

//---------------//
//----- ICU -----//

void Summary::refresh()
{
    // Frame
    m_frame.setFitSize(size());

    // Bars
    float barWidth = m_width - 2.f * m_frame.paddings().x;
    for (auto& bar : m_bars)
        if (bar != nullptr)
            bar->setWidth(barWidth);
}

void Summary::refreshFromData()
{
    m_frame.setTitle(m_data->name());

    refreshTimeBar();
    refreshDoshBar();
    refreshSoulBar();
    refreshFameBar();
}

void Summary::refreshTimeBar()
{
    auto time = m_data->time();

    static std::vector<std::wstring> months = {_("January"),    _("February"),  _("March"),     _("April"),
                                               _("May"),        _("June"),      _("July"),      _("August"),
                                               _("September"),  _("October"),   _("November"),  _("December")};

    auto day = (time / 24u) % 28u + 1u;
    auto month = (time / 672u) % 12u;
    auto year = (time / 8064u) + 7201u;

    std::wstringstream str;
    str << std::setfill(L'0') << std::setw(2) << day << L' ' << months[month] << L' ' << year;

    m_bars[BAR_TIME]->setText(str.str());
}

void Summary::refreshDoshBar()
{
    std::wstring text(L"∞");
    if (context::worlds.selected().gamemode != context::Gamemode::RICHMAN) {
        auto dosh = m_data->villain().doshWallet.value();
        text = toWString(dosh);
    }

    m_bars[BAR_DOSH]->setText(text);
}

void Summary::refreshSoulBar()
{
    std::wstring text(L"∞");
    if (context::worlds.selected().gamemode != context::Gamemode::RICHMAN) {
        auto soul = m_data->soulWallet().value();
        text = toWString(soul);
    }

    m_bars[BAR_SOUL]->setText(text);
}

void Summary::refreshFameBar()
{
    std::wstring text(L"∞");
    if (context::worlds.selected().gamemode != context::Gamemode::RICHMAN) {
        auto fame = m_data->fameWallet().value();
        text = toWString(fame);
    }

    m_bars[BAR_FAME]->setText(text);
}
