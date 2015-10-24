#include "states/menu/config.hpp"

#include "core/gettext.hpp"
#include "context/villains.hpp"
#include "tools/tools.hpp"
#include "tools/string.hpp"

using namespace states;

MenuConfig::MenuConfig(StateStack& stack)
    : baseClass(stack)
{
    // TODO Add translatable tooltips to explain each options

    // Creating scene
    auto& nuiRoot = nuiLayer().root();
    const auto& nuiSize = nuiLayer().size();

    // Background
    nuiRoot.attachChild(m_background);
    m_background.setDepth(100.f);
    m_background.setFillColor(sf::Color(0, 0, 0, 230));
    m_background.setSize(nuiSize);

    // Title
    nuiRoot.attachChild(m_title);
    m_title.setText(_("Configuration"));
    m_title.setPrestyle(scene::Label::Prestyle::MENU_TITLE);
    m_title.setRelativePosition({0.5f, 0.05f});
    m_title.centerOrigin();

    // Configuration areas
    for (auto& area : m_areas) {
        nuiRoot.attachChild(area.stacker);
        area.stacker.stackBack(area.title, nui::Align::CENTER);
        area.stacker.stackBack(area.scrollArea, nui::Align::CENTER);

        nuiRoot.attachChild(area.background);
        area.background.setDepth(100.f);
        area.background.setFillColor(sf::Color::Black);
        area.background.setOutlineColor(sf::Color::White);

        area.scrollArea.setContent(area.form);
        area.title.setPrestyle(scene::Label::Prestyle::NUI_TITLE);
    }

    // Titles
    m_areas[AreaID::GENERAL].title.setText(_("General"));
    m_areas[AreaID::GRAPHICS].title.setText(_("Graphics"));
    m_areas[AreaID::AUDIO].title.setText(_("Audio"));

    // Stackers
    m_areas[AreaID::GENERAL].stacker.setRelativeOrigin({0.5f, 0.f});
    m_areas[AreaID::GRAPHICS].stacker.setRelativeOrigin({0.5f, 0.f});
    m_areas[AreaID::AUDIO].stacker.setRelativeOrigin({0.5f, 1.f});

    m_areas[AreaID::GENERAL].stacker.setRelativePosition({0.25f, 0.15f});
    m_areas[AreaID::GRAPHICS].stacker.setRelativePosition({0.75f, 0.15f});
    m_areas[AreaID::AUDIO].stacker.setRelativePosition({0.75f, 0.85f});

    // Graphics
    m_areas[AreaID::GRAPHICS].form.add(_("Fullscreen"), m_fullscreenBox);
    m_fullscreenBox.add({_("ON"), _("OFF")});
    m_fullscreenBox.showArrows(false);

    m_areas[AreaID::GRAPHICS].form.add(_("V-sync"), m_vsyncBox);
    m_vsyncBox.add({_("ON"), _("OFF")});
    m_vsyncBox.showArrows(false);

    m_areas[AreaID::GRAPHICS].form.add(_("Antialiasing level"), m_antialiasingSelector);
    m_antialiasingSelector.setRange(0u, 4u);

    // Stacker for buttons
    nuiRoot.attachChild(m_buttonsStacker);
    m_buttonsStacker.setRelativeOrigin({0.5f, 1.f});
    m_buttonsStacker.setRelativePosition({0.5f, 0.95f});

    // Buttons
    for (auto& button : m_buttons)
        m_buttonsStacker.stackBack(button, nui::Align::OPPOSITE);

    m_buttons[0u].setAction(_("Back"), [this] { stackPop(); });
    m_buttons[1u].setAction(_("Apply changes"), [this] { applyChanges(); });

    // TODO Config content:
    // - General/UI:
    //      - Language
    //      - Scrolling factor
    //      - Zoom speed
    //      - UI Size
    //      - Font size factor
    // - Graphics:
    //      - Resolutions (between those available for full-screen)
    // - Audio:
    //      - General volume
    //      - Volume musics
    //      - Volume sounds

    refreshFormsFromConfig();
}

//-------------------//
//----- Routine -----//

bool MenuConfig::update(const sf::Time& dt)
{
    bool returnValue = baseClass::update(dt);

    // Note: stacker size recomputation was delayed to this update
    if (m_refreshBackgrounds) {
        for (auto& area : m_areas) {
            area.background.setSize(area.stacker.size());
            area.background.setLocalPosition(area.stacker.localPosition());
            area.background.setOrigin(area.stacker.getOrigin());
        }

        m_refreshBackgrounds = false;
    }

    return returnValue;
}

void MenuConfig::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    // Area backgrounds
    for (auto& area : m_areas)
        area.background.setOutlineThickness(cNUI.borderThick);
    m_refreshBackgrounds = true;
}

void MenuConfig::refreshWindow(const config::WindowInfo& cWindow)
{
    auto& resolution = cWindow.resolution;

    // Repositioning stackers and resizing elements
    m_areas[AreaID::GENERAL].scrollArea.setSize({0.48f * resolution.x, 0.66f * resolution.y});
    m_areas[AreaID::GRAPHICS].scrollArea.setSize({0.48f * resolution.x, 0.28f * resolution.y});
    m_areas[AreaID::AUDIO].scrollArea.setSize({0.48f * resolution.x, 0.28f * resolution.y});

    // TODO The current form implementation asks for size, that's not what we want here...
    // We want a dynamic height.
    m_areas[AreaID::GENERAL].form.setSize({0.48f * resolution.x, 0.80f * resolution.y});
    m_areas[AreaID::GRAPHICS].form.setSize({0.48f * resolution.x, 0.40f * resolution.y});
    m_areas[AreaID::AUDIO].form.setSize({0.48f * resolution.x, 0.40f * resolution.y});

    // Area size might have change
    m_refreshBackgrounds = true;

    baseClass::refreshWindow(cWindow);
}

void MenuConfig::handleEvent(const sf::Event& event)
{
    // Escape opens quit screen
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPop();
        return;
    }

    baseClass::handleEvent(event);
}

//-------------------------//
//----- Configuration -----//

void MenuConfig::refreshFormsFromConfig()
{
    auto& display = Application::context().display;
    // auto& sound = Application::context().sound;

    // Graphics
    m_fullscreenBox.selectChoice(display.window.fullscreen? 0u : 1u);
    m_vsyncBox.selectChoice(display.window.vsync? 0u : 1u);
    m_antialiasingSelector.setValue(display.window.antialiasingLevel);
}

void MenuConfig::applyChanges()
{
    auto& display = Application::context().display;
    auto& sound = Application::context().sound;

    // Graphics
    display.window.fullscreen = (m_fullscreenBox.selectedChoice() == 0u);
    display.window.vsync = (m_vsyncBox.selectedChoice() == 0u);
    display.window.antialiasingLevel = m_antialiasingSelector.value();

    sound.save();
    display.save();
    Application::refreshFromConfig();
}
