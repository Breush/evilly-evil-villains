#include "states/menu/newgame.hpp"

#include "core/gettext.hpp"
#include "context/villains.hpp"
#include "context/worlds.hpp"
#include "dungeon/data.hpp"
#include "tools/tools.hpp"
#include "tools/debug.hpp"
#include "tools/string.hpp"
#include "tools/vector.hpp"

using namespace states;

MenuNewGame::MenuNewGame(StateStack& stack)
    : baseClass(stack)
{
    // Loading resources
    Application::loadTextures({"villain"});

    // Creating scene
    auto& nuiRoot = nuiLayer().root();
    const auto& nuiSize = nuiLayer().size();

    // Background
    nuiRoot.attachChild(m_background);
    m_background.setDepth(100.f);
    m_background.setPointsColors({0u, 0u, 0u, 210u}, {0u, 0u, 0u, 255u});
    m_background.setSize(nuiSize);

    // Title
    nuiRoot.attachChild(m_title);
    m_title.setText(_("Select a villain to play with"));
    m_title.setPrestyle(scene::Label::Prestyle::MENU_TITLE);
    m_title.setRelativePosition({0.5f, 0.05f});
    m_title.centerOrigin();

    // Global stacker
    nuiRoot.attachChild(m_stacker);
    m_stacker.setRelativePosition({0.5f, 0.5f});
    m_stacker.centerOrigin();

    // Scroll area
    m_stacker.stackBack(m_scrollArea);
    m_scrollArea.setContent(m_scrollAreaStacker);
    m_scrollArea.setSize(nuiSize * sf::Vector2f{0.8f, 0.65f});

    // Game-mode box
    m_stacker.stackBack(m_gamemodeBox, nui::Align::CENTER);
    m_gamemodeBox.add(_("Story mode"));
    m_gamemodeBox.add(_("Rich man mode"));
    // m_gamemodeBox.add(_("Hardcore mode"));

    // Buttons
    nuiRoot.attachChild(m_buttonsStacker);
    m_buttonsStacker.setRelativeOrigin({0.5f, 1.f});
    m_buttonsStacker.setRelativePosition({0.5f, 0.95f});

    for (auto& button : m_buttons)
        m_buttonsStacker.stackBack(button, nui::Align::OPPOSITE);

    m_buttons[0].setAction(_("Back"), [this] { stackPop(); });
    m_buttons[1].setText(_("Start playing"));

    refreshVillainsList();
}

MenuNewGame::~MenuNewGame()
{
    // Freeing resources
    Application::freeTextures({"villain"});
}

//-------------------//
//----- Routine -----//

void MenuNewGame::onShow() noexcept
{
    refreshVillainsList();
}

void MenuNewGame::handleEvent(const sf::Event& event)
{
    // Escape quits current state
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPop();
        return;
    }

    State::handleEvent(event);
}

//---------------------------//
//----- Game management -----//

void MenuNewGame::startPlay()
{
    returnif (m_selectedVillainDisplay == nullptr);
    const auto& villain = m_selectedVillainDisplay->source()->name;
    auto gamemode = static_cast<context::Gamemode>(m_gamemodeBox.selectedChoice() + 1u);

    // World data
    auto newWorldID = context::worlds.add();
    auto& worldInfo = context::worlds.select(newWorldID);
    worldInfo.villain = villain;
    worldInfo.gamemode = gamemode;

    // Decide what to do given the gamemode
    if (gamemode == context::Gamemode::STORY) {
        stackClear(StateID::GAME_DCB);
    }
    else if (gamemode == context::Gamemode::RICHMAN) {
        // Max dimensions for a dungeon
        uint floorsCount = 12;
        uint roomsByFloor = 7;

        // TODO Have a state so that the player can change this name?
        // Set up the world name to match dungeon one (will create the folder too)
        const auto& name = _("Croesus Home");
        context::worlds.setNameCreation(worldInfo, name);
        context::worlds.save();

        // Create dungeon with these values
        dungeon::Data dungeonData;
        dungeonData.setName(name);
        dungeonData.setFloorsCount(floorsCount);
        dungeonData.setRoomsByFloor(roomsByFloor);
        dungeonData.fameWallet().set(-1u);
        dungeonData.soulWallet().set(-1u);
        dungeonData.constructRoomsAll();
        dungeonData.createFiles(worldInfo.folder);

        stackClear(StateID::GAME_DUNGEON_DESIGN);
    }
    else if (gamemode == context::Gamemode::HARDCORE) {
        throw std::runtime_error("Hardcore Gamemode has not been implemented yet.");
    }
}

void MenuNewGame::onVillainClicked(dungeon::VillainDisplay& clickedEntity)
{
    m_selectedVillainDisplay = &clickedEntity;

    // Select the one
    for (auto& pVillainDisplay : m_villainsDisplays)
        pVillainDisplay->setSelected(pVillainDisplay.get() == &clickedEntity);

    // Enable the start button
    m_buttons[1].setCallback([this] { startPlay(); });
}

//---------------//
//----- ICU -----//

void MenuNewGame::refreshVillainsList()
{
    m_selectedVillainDisplay = nullptr;
    m_buttons[1].setCallback(nullptr);
    m_villainsDisplays.clear();
    context::villains.load();

    // Villains
    const auto& villainsList = context::villains.get();
    for (const auto& villain : villainsList) {
        auto pVillainDisplay = std::make_unique<dungeon::VillainDisplay>();
        pVillainDisplay->setSource(villain);
        pVillainDisplay->setSize({256.f, m_scrollArea.size().y - 15.f});
        pVillainDisplay->setClickCallback([this] (dungeon::VillainDisplay& clickedEntity) { onVillainClicked(clickedEntity); });
        m_villainsDisplays.emplace_back(std::move(pVillainDisplay));
        m_scrollAreaStacker.stackBack(*m_villainsDisplays.back());
    }

    // New villain
    auto pVillainDisplay = std::make_unique<dungeon::VillainDisplay>();
    pVillainDisplay->setSize({256.f, m_scrollArea.size().y - 15.f});
    pVillainDisplay->setClickCallback([this] (dungeon::VillainDisplay&) { stackPush(StateID::MENU_CREATEVILLAIN); });
    m_villainsDisplays.emplace_back(std::move(pVillainDisplay));
    m_scrollAreaStacker.stackBack(*m_villainsDisplays.back());
}
