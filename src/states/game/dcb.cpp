#include "states/game/dcb.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "dungeon/data.hpp"
#include "context/worlds.hpp"
#include "context/villains.hpp"
#include "tools/filesystem.hpp"

using namespace states;

GameDCB::GameDCB(StateStack& stack)
    : baseClass(stack)
    , m_controller(m_gaugesManager, m_bubble, m_answerBox)
{
    // Loading resources
    Application::loadTextures({"dcb"});
    Application::loadAnimations({"dcb"});

    // Music
    Application::context().musics.play("moorland");

    // Creating scene
    auto& nuiRoot = nuiLayer().root();
    const auto& nuiSize = nuiLayer().size();
    const auto nuiRatio = nuiSize.x / nuiSize.y;

    // Background
    const auto backgroundSize = sf::v2f(Application::context().textures.get("dcb/scene/stage").getSize());
    const auto backgroundRatio = backgroundSize.x / backgroundSize.y;
    float backgroundScaleValue = 1.f;
    if (nuiRatio > backgroundRatio) backgroundScaleValue *= nuiSize.x / backgroundSize.x;
    else                            backgroundScaleValue *= nuiSize.y / backgroundSize.y;
    const sf::Vector2f backgroundScale(backgroundScaleValue, backgroundScaleValue);

    nuiRoot.attachChild(m_trees);
    m_trees.load("dcb/scene/trees");
    m_trees.setRelativePosition({0.5f, 0.5f});
    m_trees.setLocalScale(backgroundScale);

    nuiRoot.attachChild(m_stage);
    m_stage.load("dcb/scene/stage");
    m_stage.setRelativePosition({0.5f, 0.5f});
    m_stage.setLocalScale(backgroundScale);

    nuiRoot.attachChild(m_candle);
    m_candle.load("dcb/scene/candle");
    m_candle.setRelativePosition({0.5f, 0.5f});
    m_candle.setLocalScale(backgroundScale);

    // Gauges
    m_gaugesManager.setRelativePosition({0.f, 1.f});
    m_gaugesManager.setRelativeOrigin({0.f, 1.f});

    // Message
    nuiRoot.attachChild(m_bubble);
    m_bubble.setRelativePosition({1.f, 0.f});
    m_bubble.setRelativeOrigin({1.f, 0.f});
    m_bubble.setSize(nuiSize / 3.f);
    m_bubble.forceMessage(_("Welcome to the Dungeon Community Bank (DCB). "
                            "I heard that you wanted to open your own dungeon, "
                            "this is the kind of operation we like to support here.\n\n"
                            "Let me just ask you: *How do you want to name your tower of death?*"));

    // Answer box
    nuiRoot.attachChild(m_answerBox);
    m_answerBox.setSize({0.50f * nuiSize.x, 0.30f * nuiSize.y});
    m_answerBox.setRelativePosition({0.5f, 1.0f});
    m_answerBox.setRelativeOrigin({0.4f, 1.0f});

    // Entry
    m_answerBox.attachChild(m_nameEntry);
    m_nameEntry.setLength(20u);
    m_nameEntry.setRelativePosition({0.5f, 0.5f});
    m_nameEntry.centerOrigin();
    m_nameEntry.setOnValidateCallback([this] { onNameValidate(); });

    // Button
    m_answerBox.attachChild(m_okButton);
    m_okButton.setAction(_("OK"), [this] { onNameValidate(); });
    m_okButton.setLocalPosition(m_nameEntry.localPosition() + sf::Vector2f{m_nameEntry.size().x / 2.f + 10.f, 0.f});
    m_okButton.setRelativeOrigin({0.f, 0.5f});

    // Controller
    const auto& languageCode = Application::context().display.global.language;
    m_controller.setOnSequenceFinishedCallback([this] { confirmDungeonCreation(); });

    // Try to load specific language file. If none, just go back to English.
    std::string controllerFile = "res/po/" + toString(languageCode) + "/dcb.xml";
    if (fileExists(controllerFile)) m_controller.load(controllerFile);
    else m_controller.load("res/po/en_EN/dcb.xml");
}

GameDCB::~GameDCB()
{
    // Musics
    Application::context().musics.stop("moorland");

    // Freeing resources
    Application::freeTextures({"dcb"});
    Application::freeAnimations({"dcb"});
}

//-----------------------//
//----- Interaction -----//

void GameDCB::onNameValidate()
{
    returnif (m_nameEntry.text().empty());

    m_answerBox.detachChild(m_nameEntry);
    m_answerBox.detachChild(m_okButton);
    nuiLayer().root().attachChild(m_gaugesManager);

    m_controller.randomGaugesFromString(m_nameEntry.text());
    m_controller.startSequence();
}

void GameDCB::confirmDungeonCreation()
{
    auto& worldInfo = context::worlds.selected();

    // State GAMEOVER if conviction gauge has not enough
    if (!m_gaugesManager.enoughConviction()) {
        context::worlds.remove(worldInfo);
        context::worlds.save();
        stackClear(StateID::GAME_OVER);
        return;
    }

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
