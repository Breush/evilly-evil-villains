#include "dungeon/sidebar/summary.hpp"

#include "core/gettext.hpp"
#include "context/villains.hpp"
#include "context/worlds.hpp"
#include "dungeon/data.hpp"
#include "tools/random.hpp"
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
    for (auto& bar : m_rankingBars)     m_stacker.stackBack(bar);
    for (auto& bar : m_resourcesBars)   m_stacker.stackBack(bar);
}

void Summary::init()
{
    // Ranking bars
    for (auto& bar : m_rankingBars) bar.init();

    m_rankingBars[RANKING_RANK].setLogo("core/resources/evil/icon");
    m_rankingBars[RANKING_CLASS].setLogo("core/resources/fame/icon");

    m_rankingBars[RANKING_RANK].setTextColor(sf::Color::White);
    m_rankingBars[RANKING_CLASS].setTextColor({200u, 200u, 200u});

    m_rankingBars[RANKING_RANK].setProgressionColor({160u, 42u, 42u});
    m_rankingBars[RANKING_CLASS].setProgressionColor({102u, 102u, 190u});

    // Resource bars
    for (auto& bar : m_resourcesBars) bar.init();

    m_resourcesBars[RESOURCE_TIME].setLogo("core/resources/time/icon");
    m_resourcesBars[RESOURCE_DOSH].setLogo("core/resources/dosh/icon");
    m_resourcesBars[RESOURCE_SOUL].setLogo("core/resources/soul/icon");

    m_resourcesBars[RESOURCE_TIME].setColor({190u, 190u, 190u});
    m_resourcesBars[RESOURCE_DOSH].setColor({190u, 171u, 21u});
    m_resourcesBars[RESOURCE_SOUL].setColor({102u, 190u, 21u});
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
    m_resourcesBars[RESOURCE_TIME].setTooltip(_("Date and Time"));
    m_resourcesBars[RESOURCE_DOSH].setTooltip(_("Dosh"));
    m_resourcesBars[RESOURCE_SOUL].setTooltip(_("Souls"));

    // TODO Should refresh ranking bars for translations

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
        refreshClassBar();
    else if (devent.type == "evil_changed")
        refreshRankBar();
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
    for (auto& bar : m_rankingBars)     bar.setWidth(barWidth);
    for (auto& bar : m_resourcesBars)   bar.setWidth(barWidth);
}

void Summary::refreshFromData()
{
    m_frame.setTitle(m_data->name());

    refreshTimeBar();
    refreshDoshBar();
    refreshSoulBar();
    refreshRankBar();
    refreshClassBar();
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

    m_resourcesBars[RESOURCE_TIME].setText(str.str());
}

void Summary::refreshDoshBar()
{
    std::wstring text(L"∞");
    if (context::worlds.selected().gamemode != context::Gamemode::RICHMAN) {
        auto dosh = m_data->villain().doshWallet.value();
        text = toWString(dosh);
    }

    m_resourcesBars[RESOURCE_DOSH].setText(text);
}

void Summary::refreshSoulBar()
{
    std::wstring text(L"∞");
    if (context::worlds.selected().gamemode != context::Gamemode::RICHMAN) {
        auto soul = m_data->soulWallet().value();
        text = toWString(soul);
    }

    m_resourcesBars[RESOURCE_SOUL].setText(text);
}

void Summary::refreshRankBar()
{
    // TODO Select rank name given the current villain's evilness
    auto rankNames = {_("Teddy bear"), _("Casual villain"), _("Baby Jafar")};

    m_rankingBars[RANKING_RANK].setText(alea::rand(rankNames));
    m_rankingBars[RANKING_RANK].setPercent(0.75f);
}

void Summary::refreshClassBar()
{
    constexpr uint FAME_LEVEL_BASE = 5u;

    auto fame = m_data->fameWallet().value();
    auto level = static_cast<uint>(std::log(fame) / std::log(FAME_LEVEL_BASE));

    std::wstring text(L"∞");
    if (context::worlds.selected().gamemode != context::Gamemode::RICHMAN)
        text = L"Level " + toWString(level);

    auto levelLimit = std::pow(FAME_LEVEL_BASE, level);
    auto levelSize = std::pow(FAME_LEVEL_BASE, level + 1u) - levelLimit;

    m_rankingBars[RANKING_CLASS].setText(text);
    m_rankingBars[RANKING_CLASS].setPercent((fame - levelLimit) / levelSize);
}
