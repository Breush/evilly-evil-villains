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
        nuiRoot.attachChild(area.frame);
        area.frame.setContent(area.scrollArea);
        area.scrollArea.setContent(area.form);
    }

    // Stackers
    m_areas[AreaID::GENERAL].frame.setRelativeOrigin({0.5f, 0.f});
    m_areas[AreaID::GRAPHICS].frame.setRelativeOrigin({0.5f, 0.f});
    m_areas[AreaID::AUDIO].frame.setRelativeOrigin({0.5f, 1.f});

    m_areas[AreaID::GENERAL].frame.setRelativePosition({0.25f, 0.15f});
    m_areas[AreaID::GRAPHICS].frame.setRelativePosition({0.75f, 0.15f});
    m_areas[AreaID::AUDIO].frame.setRelativePosition({0.75f, 0.85f});

    // General
    m_areas[AreaID::GENERAL].form.add(m_languageList);
    for (const auto& language : i18n::languagesList())
        m_languageList.add(language.name);

    m_areas[AreaID::GENERAL].form.add(m_uiSizeSlider);
    m_uiSizeSlider.setRange(1u, 4u);

    m_areas[AreaID::GENERAL].form.add(m_uiFontFactorSlider);
    m_uiFontFactorSlider.setVisibleSteps(2u);
    m_uiFontFactorSlider.setRange(50u, 150u);

    m_areas[AreaID::GENERAL].form.add(m_scrollingFactorSlider);
    m_scrollingFactorSlider.setVisibleSteps(3u);
    m_scrollingFactorSlider.setRange(5u, 50u);

    m_areas[AreaID::GENERAL].form.add(m_zoomSpeedSlider);
    m_zoomSpeedSlider.setVisibleSteps(2u);
    m_zoomSpeedSlider.setRange(1u, 9u);

    // Graphics
    m_areas[AreaID::GRAPHICS].form.add(m_resolutionList);
    auto refBitsPerPixel = sf::VideoMode::getDesktopMode().bitsPerPixel;
    for (const auto& videoMode : sf::VideoMode::getFullscreenModes()) {
        if (refBitsPerPixel != videoMode.bitsPerPixel) continue;
        if (static_cast<float>(videoMode.width) / static_cast<float>(videoMode.height) < 1.3f) continue;

        Resolution resolution{videoMode.width, videoMode.height};
        m_resolutions.emplace_back(resolution);

        std::wstring sResolution = toWString(resolution.x) + L'x' + toWString(resolution.y);
        sResolution += L" (" + aspectRatio(resolution.x, resolution.y) + L')';
        m_resolutionList.add(sResolution);
    }

    m_areas[AreaID::GRAPHICS].form.add(m_fullscreenBox);

    m_areas[AreaID::GRAPHICS].form.add(m_vsyncBox);

    m_areas[AreaID::GRAPHICS].form.add(m_antialiasingSlider);
    m_antialiasingSlider.setRange(0u, 4u);

    // Audio
    m_areas[AreaID::AUDIO].form.add(m_globalVolumeSlider);
    m_globalVolumeSlider.setVisibleSteps(4u);
    m_globalVolumeSlider.setRange(0u, 100u);

    m_areas[AreaID::AUDIO].form.add(m_musicVolumeSlider);
    m_musicVolumeSlider.setVisibleSteps(4u);
    m_musicVolumeSlider.setRange(0u, 100u);

    m_areas[AreaID::AUDIO].form.add(m_soundVolumeSlider);
    m_soundVolumeSlider.setVisibleSteps(4u);
    m_soundVolumeSlider.setRange(0u, 100u);

    // Stacker for buttons
    nuiRoot.attachChild(m_buttonsStacker);
    m_buttonsStacker.setRelativeOrigin({0.5f, 1.f});
    m_buttonsStacker.setRelativePosition({0.5f, 0.95f});

    // Buttons
    for (auto& button : m_buttons)
        m_buttonsStacker.stackBack(button, nui::Align::OPPOSITE);

    m_buttons[0u].setValidateCallback([this] { stackPop(); });
    m_buttons[1u].setValidateCallback([this] { applyChanges(); });

    refreshFormsFromConfig();
}

//-------------------//
//----- Routine -----//

void MenuConfig::refreshWindow(const config::WindowInfo& cWindow)
{
    const auto& resolution = cWindow.resolution;

    // Translated strings
    m_title.setText(_("Configuration"));

    m_areas[AreaID::GENERAL].frame.setTitle(_("General"));
    m_areas[AreaID::GENERAL].form.setText(0u, _("Language"));
    m_areas[AreaID::GENERAL].form.setText(1u, _("Interface size"));
    m_areas[AreaID::GENERAL].form.setText(2u, _("Font size (%)"));
    m_areas[AreaID::GENERAL].form.setText(3u, _("Scrolling speed"));
    m_areas[AreaID::GENERAL].form.setText(4u, _("Zoom speed"));

    m_areas[AreaID::GRAPHICS].frame.setTitle(_("Graphics"));
    m_areas[AreaID::GRAPHICS].form.setText(0u, _("Resolution"));
    m_areas[AreaID::GRAPHICS].form.setText(1u, _("Fullscreen"));
    m_areas[AreaID::GRAPHICS].form.setText(2u, _("V-sync"));
    m_areas[AreaID::GRAPHICS].form.setText(3u, _("Antialiasing level"));

    m_areas[AreaID::AUDIO].frame.setTitle(_("Audio"));
    m_areas[AreaID::AUDIO].form.setText(0u, _("Global volume"));
    m_areas[AreaID::AUDIO].form.setText(1u, _("Music volume"));
    m_areas[AreaID::AUDIO].form.setText(2u, _("Sounds volume"));

    m_buttons[0u].setText(_("Back"));
    m_buttons[1u].setText(_("Apply changes"));

    // Background
    m_background.setSize(resolution);

    // Repositioning stackers and resizing elements
    m_areas[AreaID::GENERAL].scrollArea.setSize({0.45f * resolution.x, 0.645f * resolution.y});
    m_areas[AreaID::GRAPHICS].scrollArea.setSize({0.45f * resolution.x, 0.29f * resolution.y});
    m_areas[AreaID::AUDIO].scrollArea.setSize({0.45f * resolution.x, 0.29f * resolution.y});

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
    auto languageIndex = i18n::languageIndexFromCode(display.global.language);
    m_languageList.select(languageIndex);
    m_uiSizeSlider.setValue(display.nui.size);
    m_uiFontFactorSlider.setValue(static_cast<uint>(display.nui.fontFactor * 100.f));
    m_scrollingFactorSlider.setValue(static_cast<uint>(display.global.scrollingFactor));
    m_zoomSpeedSlider.setValue(static_cast<uint>(display.global.zoomSpeed * 100.f));

    // Graphics
    auto resolution = sf::v2u(display.window.resolution);
    for (uint i = 0u; i < m_resolutions.size(); ++i) {
        const auto& availableResolution = m_resolutions[i];
        if (availableResolution == resolution) {
            m_resolutionList.select(i);
            break;
        }
    }

    m_fullscreenBox.setStatus(display.window.fullscreen);
    m_vsyncBox.setStatus(display.window.vsync);
    m_antialiasingSlider.setValue(display.window.antialiasingLevel);

    // Audio
    m_globalVolumeSlider.setValue(static_cast<uint>(audio.globalRelVolume * 100.f));
    m_musicVolumeSlider.setValue(static_cast<uint>(audio.musicVolume));
    m_soundVolumeSlider.setValue(static_cast<uint>(audio.soundVolume));
}

void MenuConfig::applyChanges()
{
    auto& display = Application::context().display;
    auto& audio = Application::context().sound;

    // General
    display.global.language = i18n::languagesList().at(m_languageList.selected()).code;
    display.nui.size = m_uiSizeSlider.value();
    display.nui.fontFactor = m_uiFontFactorSlider.value() / 100.f;
    display.global.scrollingFactor = m_scrollingFactorSlider.value();
    display.global.zoomSpeed = m_zoomSpeedSlider.value() / 100.f;

    // Graphics
    auto resolution = sf::v2f(m_resolutions.at(m_resolutionList.selected()));
    auto fullscreen = m_fullscreenBox.status();
    auto vsync = m_vsyncBox.status();
    auto antialiasingLevel = m_antialiasingSlider.value();

    bool windowRefresh = (display.window.resolution != resolution)
                      || (display.window.fullscreen != fullscreen)
                      || (display.window.vsync != vsync)
                      || (display.window.antialiasingLevel != antialiasingLevel);

    display.window.resolution = resolution;
    display.window.fullscreen = fullscreen;
    display.window.vsync = vsync;
    display.window.antialiasingLevel = antialiasingLevel;

    // Audio
    // TODO It might be good UX to let the user hear the audio differences live,
    // and not delay the update until here.
    audio.globalRelVolume = m_globalVolumeSlider.value() / 100.f;
    audio.musicVolume = m_musicVolumeSlider.value();
    audio.soundVolume = m_soundVolumeSlider.value();

    audio.save();
    display.save();
    Application::refreshFromConfig(windowRefresh, true);
}
