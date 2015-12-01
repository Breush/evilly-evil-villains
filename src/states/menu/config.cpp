#include "states/menu/config.hpp"

#include "core/gettext.hpp"
#include "context/villains.hpp"
#include "tools/math.hpp"
#include "tools/tools.hpp"
#include "tools/string.hpp"
#include "tools/filesystem.hpp"

using namespace states;

//----------------------//
//----- MenuConfig -----//

MenuConfig::MenuConfig(StateStack& stack)
    : baseClass(stack)
{
    // TODO Add translatable tooltips to explain each options

    // Creating scene
    auto& nuiRoot = nuiLayer().root();

    // Background
    nuiRoot.attachChild(m_background);
    m_background.setDepth(100.f);
    m_background.setPointsColors({0u, 0u, 0u, 210u}, {0u, 0u, 0u, 255u});

    // Title
    nuiRoot.attachChild(m_title);
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
        area.background.setFillColor({0u, 0u, 0u, 128u});
        area.background.setOutlineColor(sf::Color::White);

        area.scrollArea.setContent(area.form);
        area.title.setPrestyle(scene::Label::Prestyle::NUI_TITLE);
    }

    // Stackers
    m_areas[AreaID::GENERAL].stacker.setRelativeOrigin({0.5f, 0.f});
    m_areas[AreaID::GRAPHICS].stacker.setRelativeOrigin({0.5f, 0.f});
    m_areas[AreaID::AUDIO].stacker.setRelativeOrigin({0.5f, 1.f});

    m_areas[AreaID::GENERAL].stacker.setRelativePosition({0.25f, 0.15f});
    m_areas[AreaID::GRAPHICS].stacker.setRelativePosition({0.75f, 0.15f});
    m_areas[AreaID::AUDIO].stacker.setRelativePosition({0.75f, 0.85f});

    // General
    m_areas[AreaID::GENERAL].form.add(m_languageBox);
    for (auto& fileInfo : listFiles("res/po"))
        if (fileInfo.isDirectory)
            m_languageBox.add(toWString(replace(fileInfo.name, std::string("_"), std::string("\\_"))));

    m_areas[AreaID::GENERAL].form.add(m_uiSizeSelector);
    m_uiSizeSelector.setRange(1u, 4u);

    m_areas[AreaID::GENERAL].form.add(m_uiFontFactorSelector);
    m_uiFontFactorSelector.setVisibleSteps(2u);
    m_uiFontFactorSelector.setRange(50u, 150u);

    m_areas[AreaID::GENERAL].form.add(m_scrollingFactorSelector);
    m_scrollingFactorSelector.setVisibleSteps(3u);
    m_scrollingFactorSelector.setRange(5u, 50u);

    m_areas[AreaID::GENERAL].form.add(m_zoomSpeedSelector);
    m_zoomSpeedSelector.setVisibleSteps(2u);
    m_zoomSpeedSelector.setRange(1u, 9u);

    // Graphics
    m_areas[AreaID::GRAPHICS].form.add(m_resolutionBox);
    auto refBitsPerPixel = sf::VideoMode::getFullscreenModes().at(0u).bitsPerPixel;
    for (const auto& videoMode : std::reverse(sf::VideoMode::getFullscreenModes())) {
        // Best bits per pixels are at the end with std::reverse
        if (refBitsPerPixel != videoMode.bitsPerPixel) continue;

        Resolution resolution{videoMode.width, videoMode.height};
        m_resolutions.emplace_back(resolution);

        std::wstring sResolution = toWString(resolution.x) + L'x' + toWString(resolution.y);
        sResolution += L" (" + aspectRatio(resolution.x, resolution.y) + L')';
        m_resolutionBox.add(sResolution);
    }

    m_areas[AreaID::GRAPHICS].form.add(m_fullscreenBox);
    m_fullscreenBox.showArrows(false);
    m_fullscreenBox.add({L"", L""});

    m_areas[AreaID::GRAPHICS].form.add(m_vsyncBox);
    m_vsyncBox.showArrows(false);
    m_vsyncBox.add({L"", L""});

    m_areas[AreaID::GRAPHICS].form.add(m_antialiasingSelector);
    m_antialiasingSelector.setRange(0u, 4u);

    // Audio
    m_areas[AreaID::AUDIO].form.add(m_globalVolumeSelector);
    m_globalVolumeSelector.setVisibleSteps(4u);
    m_globalVolumeSelector.setRange(0u, 100u);

    m_areas[AreaID::AUDIO].form.add(m_musicVolumeSelector);
    m_musicVolumeSelector.setVisibleSteps(4u);
    m_musicVolumeSelector.setRange(0u, 100u);

    m_areas[AreaID::AUDIO].form.add(m_soundVolumeSelector);
    m_soundVolumeSelector.setVisibleSteps(4u);
    m_soundVolumeSelector.setRange(0u, 100u);

    // Stacker for buttons
    nuiRoot.attachChild(m_buttonsStacker);
    m_buttonsStacker.setRelativeOrigin({0.5f, 1.f});
    m_buttonsStacker.setRelativePosition({0.5f, 0.95f});

    // Buttons
    for (auto& button : m_buttons)
        m_buttonsStacker.stackBack(button, nui::Align::OPPOSITE);

    m_buttons[0u].setCallback([this] { stackPop(); });
    m_buttons[1u].setCallback([this] { applyChanges(); });

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
    const auto& resolution = cWindow.resolution;

    // Translated strings
    m_title.setText(_("Configuration"));

    m_areas[AreaID::GENERAL].title.setText(_("General"));
    m_areas[AreaID::GRAPHICS].title.setText(_("Graphics"));
    m_areas[AreaID::AUDIO].title.setText(_("Audio"));

    m_areas[AreaID::GENERAL].form.setText(0u, _("Language"));
    m_areas[AreaID::GENERAL].form.setText(1u, _("Interface size"));
    m_areas[AreaID::GENERAL].form.setText(2u, _("Font size (%)"));
    m_areas[AreaID::GENERAL].form.setText(3u, _("Scrolling speed"));
    m_areas[AreaID::GENERAL].form.setText(4u, _("Zoom speed"));

    m_areas[AreaID::GRAPHICS].form.setText(0u, _("Resolution"));
    m_areas[AreaID::GRAPHICS].form.setText(1u, _("Fullscreen"));
    m_areas[AreaID::GRAPHICS].form.setText(2u, _("V-sync"));
    m_areas[AreaID::GRAPHICS].form.setText(3u, _("Antialiasing level"));

    m_areas[AreaID::AUDIO].form.setText(0u, _("Global volume"));
    m_areas[AreaID::AUDIO].form.setText(1u, _("Music volume"));
    m_areas[AreaID::AUDIO].form.setText(2u, _("Sounds volume"));

    m_fullscreenBox.setChoiceText(0u, _("ON"));
    m_fullscreenBox.setChoiceText(1u, _("OFF"));

    m_vsyncBox.setChoiceText(0u, _("ON"));
    m_vsyncBox.setChoiceText(1u, _("OFF"));

    m_buttons[0u].setText(_("Back"));
    m_buttons[1u].setText(_("Apply changes"));

    // Background
    m_background.setSize(resolution);

    // Repositioning stackers and resizing elements
    m_areas[AreaID::GENERAL].scrollArea.setSize({0.48f * resolution.x, 0.66f * resolution.y});
    m_areas[AreaID::GRAPHICS].scrollArea.setSize({0.48f * resolution.x, 0.28f * resolution.y});
    m_areas[AreaID::AUDIO].scrollArea.setSize({0.48f * resolution.x, 0.28f * resolution.y});

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
    auto& audio = Application::context().sound;

    // General
    m_languageBox.selectChoice(replace(display.global.language, std::wstring(L"_"), std::wstring(L"\\_")));
    m_uiSizeSelector.setValue(display.nui.size);
    m_uiFontFactorSelector.setValue(static_cast<uint>(display.nui.fontFactor * 100.f));
    m_scrollingFactorSelector.setValue(static_cast<uint>(display.global.scrollingFactor));
    m_zoomSpeedSelector.setValue(static_cast<uint>(display.global.zoomSpeed * 100.f));

    // Graphics
    auto resolution = sf::v2u(display.window.resolution);
    for (uint i = 0u; i < m_resolutions.size(); ++i) {
        const auto& availableResolution = m_resolutions[i];
        if (availableResolution == resolution) {
            m_resolutionBox.selectChoice(i);
            break;
        }
    }

    m_fullscreenBox.selectChoice(display.window.fullscreen? 0u : 1u);
    m_vsyncBox.selectChoice(display.window.vsync? 0u : 1u);
    m_antialiasingSelector.setValue(display.window.antialiasingLevel);

    // Audio
    m_globalVolumeSelector.setValue(static_cast<uint>(audio.globalRelVolume * 100.f));
    m_musicVolumeSelector.setValue(static_cast<uint>(audio.musicVolume));
    m_soundVolumeSelector.setValue(static_cast<uint>(audio.soundVolume));
}

void MenuConfig::applyChanges()
{
    auto& display = Application::context().display;
    auto& audio = Application::context().sound;

    // General
    display.global.language = replace(m_languageBox.selectedChoiceText(), std::wstring(L"\\_"), std::wstring(L"_"));
    display.nui.size = m_uiSizeSelector.value();
    display.nui.fontFactor = m_uiFontFactorSelector.value() / 100.f;
    display.global.scrollingFactor = m_scrollingFactorSelector.value();
    display.global.zoomSpeed = m_zoomSpeedSelector.value() / 100.f;

    // Graphics
    display.window.resolution = sf::v2f(m_resolutions.at(m_resolutionBox.selectedChoice()));
    display.window.fullscreen = (m_fullscreenBox.selectedChoice() == 0u);
    display.window.vsync = (m_vsyncBox.selectedChoice() == 0u);
    display.window.antialiasingLevel = m_antialiasingSelector.value();

    // Audio
    audio.globalRelVolume = m_globalVolumeSelector.value() / 100.f;
    audio.musicVolume = m_musicVolumeSelector.value();
    audio.soundVolume = m_soundVolumeSelector.value();

    audio.save();
    display.save();
    Application::refreshFromConfig();
}
