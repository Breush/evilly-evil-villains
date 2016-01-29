#include "states/game/dcb.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "dungeon/data.hpp"
#include "context/worlds.hpp"
#include "context/context.hpp"
#include "context/villains.hpp"
#include "tools/filesystem.hpp"

using namespace states;

GameDCB::GameDCB(StateStack& stack)
    : baseClass(stack)
    , m_controller(m_gaugesManager, m_bubble, m_answerBox)
{
    // Loading resources
    Application::loadTextures({"core/menu/dcb"});
    Application::loadAnimations({"core/menu/dcb"});

    // Music
    context::context.musics.play("core/global/musics/moorland");

    // Creating scene
    auto& nuiRoot = nuiLayer().root();
    const auto& nuiSize = nuiLayer().size();
    const auto nuiRatio = nuiSize.x / nuiSize.y;

    // Background
    const auto backgroundSize = sf::v2f(context::context.textures.get("core/menu/dcb/stage/stage").getSize());
    const auto backgroundRatio = backgroundSize.x / backgroundSize.y;
    float backgroundScaleValue = 1.f;
    if (nuiRatio > backgroundRatio) backgroundScaleValue *= nuiSize.x / backgroundSize.x;
    else                            backgroundScaleValue *= nuiSize.y / backgroundSize.y;
    const sf::Vector2f backgroundScale(backgroundScaleValue, backgroundScaleValue);

    nuiRoot.attachChild(m_trees);
    m_trees.load("core/menu/dcb/trees");
    m_trees.setRelativePosition({0.5f, 0.5f});
    m_trees.setLocalScale(backgroundScale);

    nuiRoot.attachChild(m_stage);
    m_stage.load("core/menu/dcb/stage");
    m_stage.setRelativePosition({0.5f, 0.5f});
    m_stage.setLocalScale(backgroundScale);

    nuiRoot.attachChild(m_candle);
    m_candle.load("core/menu/dcb/candle");
    m_candle.setRelativePosition({0.5f, 0.5f});
    m_candle.setLocalScale(backgroundScale);

    // Gauges
    m_gaugesManager.setRelativePosition({0.f, 1.f});
    m_gaugesManager.setRelativeOrigin({0.f, 1.f});

    // Message
    nuiRoot.attachChild(m_bubble);
    m_bubble.setRelativePosition({0.99f, 0.01f});
    m_bubble.setRelativeOrigin({1.f, 0.f});
    m_bubble.fitWidth(0.3f * nuiSize.x);
    m_bubble.forceMessage(_("Welcome to the Dungeon Community Bank (DCB). "
                            "I heard that you wanted to open your own dungeon, "
                            "this is the kind of operation we like to support here.\n\n"
                            "Let me just ask you: *How do you want to name your tower of death?*"));

    // Answer box
    nuiRoot.attachChild(m_answerBox);
    m_answerBox.setSize({0.40f * nuiSize.x, 0.30f * nuiSize.y});
    m_answerBox.setRelativePosition({0.5f, 1.0f});
    m_answerBox.setRelativeOrigin({0.5f, 1.0f});

    // Name
    m_answerBox.attachChild(m_nameStacker);
    m_nameStacker.stackBack(m_nameEntry);
    m_nameStacker.stackBack(m_okButton);
    m_nameStacker.setRelativePosition({0.5f, 0.5f});
    m_nameStacker.centerOrigin();

    m_nameEntry.setLength(20u);
    m_nameEntry.setOnValidateCallback([this] { onNameValidate(); });
    m_okButton.set(_("OK"), [this] { onNameValidate(); });

    // Controller
    const auto& languageCode = context::context.display.global.language;
    m_controller.setOnSequenceFinishedCallback([this] { beforeConfirmDungeonCreation(); });

    // Try to load specific language file. If none, just go back to English.
    std::string controllerFile = "res/core/langs/" + toString(languageCode) + "/dcb.xml";
    if (fileExists(controllerFile)) m_controller.load(controllerFile);
    else m_controller.load("res/core/langs/en_EN/dcb.xml");
}

GameDCB::~GameDCB()
{
    // Musics
    context::context.musics.stop("core/global/musics/moorland");

    // Freeing resources
    Application::freeTextures({"core/menu/dcb"});
    Application::freeAnimations({"core/menu/dcb"});
}

//------------------//
//----- Events -----//

void GameDCB::handleEvent(const sf::Event& event)
{
    // TODO Have a GameQuitDCB state as the Pause's text is not what we want,
    // and we need to clean context::worlds (as done for the game over)
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape)
            stackPush(StateID::GAME_PAUSE);
    }
    else if (event.type == sf::Event::MouseButtonPressed) {
        if (m_confirmGoing) {
            if (m_confirmStatus) {
                confirmDungeonCreation();
            }
            else {
                // Game over if conviction gauge has not enough
                auto& worldInfo = context::worlds.selected();
                context::worlds.remove(worldInfo);
                context::worlds.save();
                stackClear(StateID::GAME_OVER);
            }
        }
    }


    baseClass::handleEvent(event);
}

//-----------------------//
//----- Interaction -----//

void GameDCB::onNameValidate()
{
    returnif (m_nameEntry.text().empty());

    m_answerBox.detachChild(m_nameStacker);
    nuiLayer().root().attachChild(m_gaugesManager);

    m_controller.randomGaugesFromString(m_nameEntry.text());
    m_controller.startSequence();
}

void GameDCB::beforeConfirmDungeonCreation()
{
    m_confirmGoing = true;
    m_confirmStatus = m_gaugesManager.enoughConviction();

    nuiLayer().root().detachChild(m_gaugesManager);
    nuiLayer().root().detachChild(m_answerBox);

    if (m_confirmStatus)
        m_bubble.forceMessage(_("I think you have successfully convinced me. "
                                "I do believe in your project. "
                                "Let me check a few details, and see you soon in your new home!\n\n"
                                "*Click this message to get to your dungeon.*"));
    else
        m_bubble.forceMessage(_("I am really sorry, but... I don't feel like you're worth it. "
                                "I don't think you would be so great as a dungeon master...\n\n"
                                "*Click this message to end the conversation.*"));
}

void GameDCB::confirmDungeonCreation()
{
    auto& worldInfo = context::worlds.selected();

    // Affects different creation
    auto appreciation = m_gaugesManager.gaugeValue(dcb::GaugesManager::GaugeID::APPRECIATION);
    auto confusion = m_gaugesManager.gaugeValue(dcb::GaugesManager::GaugeID::CONFUSION);
    auto trust = m_gaugesManager.gaugeValue(dcb::GaugesManager::GaugeID::TRUST);
    auto conviction = m_gaugesManager.gaugeValue(dcb::GaugesManager::GaugeID::CONVICTION);

    const auto& name = m_nameEntry.text();
    uint loanValue = static_cast<uint>(1024.f + 2.f * appreciation * appreciation);
    uint floorsCount = static_cast<uint>(1.f + 16.f * (trust + conviction) / 200.f);
    uint roomsByFloor = static_cast<uint>(2.f + 6.f * (trust * conviction) / 10000.f);

    float interestRate = 1.05f + 1.f / static_cast<float>(1u + confusion);
    uint debtWeeksDuration = 24u;
    uint totalDebt = static_cast<uint>(loanValue * interestRate);
    uint weeklyDebt = totalDebt / debtWeeksDuration;

    // Set up the world name to match dungeon one (will create the folder too)
    context::worlds.setNameCreation(worldInfo, name);
    context::worlds.save();

    // Create dungeon with these values
    dungeon::Data dungeonData;
    dungeonData.setName(name);
    dungeonData.setFloorsCount(floorsCount);
    dungeonData.setRoomsByFloor(roomsByFloor);
    dungeonData.setDebt(weeklyDebt, debtWeeksDuration);
    dungeonData.createFiles(worldInfo.folder);

    // Add loan to villain ressources
    auto& villain = *context::villains.getFromVillainName(worldInfo.villain);
    villain.doshWallet.add(loanValue);
    context::villains.save();

    stackClear(StateID::GAME_DUNGEON_DESIGN);
}
