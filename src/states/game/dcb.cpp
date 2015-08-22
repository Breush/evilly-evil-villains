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
    // Creating scene
    auto& nuiRoot = nuiLayer().root();
    const auto& nuiSize = nuiLayer().size();

    // Background
    const auto& backgroundSize = Application::context().textures.get("dcb/scene/background").getSize();
    nuiRoot.attachChild(m_background);
    m_background.load("dcb/scene");
    m_background.setRelativePosition({0.5f, 0.5f});
    // TODO Currently only OK for 16:9 resolutions
    m_background.setLocalScale(nuiSize / sf::v2f(backgroundSize));

    // Gauges
    m_gaugesManager.setRelativePosition({0.f, 0.80f});
    m_gaugesManager.setRelativeOrigin({0.f, 0.50f});

    // Message
    nuiRoot.attachChild(m_bubble);
    m_bubble.setRelativePosition({1.f, 0.f});
    m_bubble.setRelativeOrigin({1.f, 0.f});
    m_bubble.setSize(nuiSize / 3.f);
    m_bubble.forceMessage(_("Welcome to the Dungeon Community Bank (DCB). "
                            "I heard that you wanted to open your own dungeon, "
                            "this is the kind of operation we like to support here.\n\n"
                            "Let me just ask you: How do you want to name your tower of death?"));

    // Answer box
    nuiRoot.attachChild(m_answerBox);
    m_answerBox.setSize({0.75f * nuiSize.x, 0.40f * nuiSize.y});
    m_answerBox.setRelativePosition({0.25f, 0.60f});

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
    // TODO State GAMEOVER if conviction gauge has not enough

    // Affects different creation
    auto appreciation = m_gaugesManager.gaugeValue(dcb::GaugesManager::GaugeID::APPRECIATION);
    //auto confusion = m_gaugesManager.gaugeValue(dcb::GaugesManager::GaugeID::CONFUSION);
    auto trust = m_gaugesManager.gaugeValue(dcb::GaugesManager::GaugeID::TRUST);
    auto conviction = m_gaugesManager.gaugeValue(dcb::GaugesManager::GaugeID::CONVICTION);

    const auto& name = m_nameEntry.text();
    uint loanValue = static_cast<uint>(1024.f + 2.f * appreciation * appreciation);
    //float interestRate = 1.05f + 1.f / static_cast<float>(1u + confusion);
    uint floorsCount = static_cast<uint>(17.f * (trust + conviction) / 200.f);
    uint roomsByFloor = static_cast<uint>(2.f + 6.f * (trust * conviction) / 10000.f);

    // Create dungeon with these values
    dungeon::Data dungeonData;
    const auto& worldInfo = context::worlds.selected();

    dungeonData.setName(name);
    dungeonData.setFloorsCount(floorsCount);
    dungeonData.setRoomsByFloor(roomsByFloor);
    // TODO dungeonData.setInterestRate(interestRate);
    dungeonData.createFiles(worldInfo.folder);

    // Add loan to villain ressources
    auto villain = context::villains.getFromWorldFolder(worldInfo.folder);
    villain->doshWallet.add(loanValue);
    context::villains.save();

    stackClear(StateID::GAME_DUNGEON_DESIGN);
}
