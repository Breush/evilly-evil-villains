#include "states/menu/config.hpp"

#include "core/gettext.hpp"
#include "core/application.hpp"
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

    m_areas[AreaID::GENERAL].frame.setRelativePosition({0.25f, 0.14f});
    m_areas[AreaID::GRAPHICS].frame.setRelativePosition({0.75f, 0.14f});
    m_areas[AreaID::AUDIO].frame.setRelativePosition({0.75f, 0.87f});

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
    for (uint sliderIndex = 0u; sliderIndex < m_volumeSliders.size(); ++sliderIndex) {
        auto& volumeSlider = m_volumeSliders[sliderIndex];

        m_areas[AreaID::AUDIO].form.add(volumeSlider);
        volumeSlider.setVisibleSteps(4u);
        volumeSlider.setRange(0u, 100u);
        volumeSlider.setValueChangedCallback([this, sliderIndex] (uint) { onVolumeSliderChanged(sliderIndex); });
    }

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

    // Translated tooltips
    m_areas[AreaID::GENERAL].form.setTooltip(0u, _("The language to use for all texts."));
    m_areas[AreaID::GENERAL].form.setTooltip(1u, _("The global size of the User Interface.\nIf you play with a small resolution,\nyou might want to set it smaller."));
    m_areas[AreaID::GENERAL].form.setTooltip(2u, _("The size factor to apply to all texts.\nThis is a good option for video makers,\nas any text will become bigger."));
    m_areas[AreaID::GENERAL].form.setTooltip(3u, _("The speed when map-scrolling\nduring dungeon design."));
    m_areas[AreaID::GENERAL].form.setTooltip(4u, _("The speed when zooming in and out\nduring dungeon design."));

    m_areas[AreaID::GRAPHICS].form.setTooltip(0u, _("Fixed resolution of the game.\nIn fullscreen mode, this will change\nyour desktop resolution for the duration of the game."));
    m_areas[AreaID::GRAPHICS].form.setTooltip(1u, _("Whether we are in fullscreen\nor in windowed mode."));
    m_areas[AreaID::GRAPHICS].form.setTooltip(2u, _("Vertical synchronisation.\nIf active, the game tries to match you monitor refresh rate.\nThis is usually a good thing to enable because it fixes screen tearing.\nHowever, on older computers, this can be a source of inputs lag."));
    m_areas[AreaID::GRAPHICS].form.setTooltip(3u, _("0 to deactivate anti-aliasing.\nIf active, the game smoothes the edges.\nOn older computers, this can be a source of render lag or have no effect."));

    m_areas[AreaID::AUDIO].form.setTooltip(0u, _("General volume factor."));
    m_areas[AreaID::AUDIO].form.setTooltip(1u, _("Music volume factor."));
    m_areas[AreaID::AUDIO].form.setTooltip(2u, _("Sounds volume factor."));

    // Background
    m_background.setSize(resolution);

    // Repositioning stackers and resizing elements
    m_areas[AreaID::GENERAL].scrollArea.setSize({0.45f * resolution.x, 0.64f * resolution.y});
    m_areas[AreaID::GRAPHICS].scrollArea.setSize({0.45f * resolution.x, 0.27f * resolution.y});
    m_areas[AreaID::AUDIO].scrollArea.setSize({0.45f * resolution.x, 0.27f * resolution.y});

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
    auto& display = context::context.display;
    auto& audio = context::context.audio;

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
    m_volumeSliders[0u].setValue(static_cast<uint>(audio.globalRelVolume * 100.f), false);
    m_volumeSliders[1u].setValue(static_cast<uint>(audio.musicVolume), false);
    m_volumeSliders[2u].setValue(static_cast<uint>(audio.soundVolume), false);
}

void MenuConfig::applyChanges()
{
    auto& display = context::context.display;

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
    // So far, it's all updated directly
    display.save();
    Application::refreshFromConfig(windowRefresh, false);
}

void MenuConfig::onVolumeSliderChanged(uint sliderIndex)
{
    auto& audio = context::context.audio;

    if (sliderIndex == 0u)      audio.globalRelVolume = m_volumeSliders[0u].value() / 100.f;
    else if (sliderIndex == 1u) audio.musicVolume = m_volumeSliders[1u].value();
    else if (sliderIndex == 2u) audio.soundVolume = m_volumeSliders[2u].value();

    audio.save();
    // TODO That's not a very well designed function...
    Application::refreshFromConfig(false, true);

    // Play testing sound so that the user can hear the level of sounds
    if (sliderIndex == 2u)
        context::context.sounds.play("core/nui/select/select");
}
