#include "core/application.hpp"

#include "resources/identifiers.hpp"

void Application::loadSounds()
{
    // Splash-screen
    s_context.sounds.load(SoundID::JUMPINGTOASTS, "res/snd/jumping-toasts.wav", true);

    // NUI
    s_context.sounds.load(SoundID::NUI_ACCEPT, "res/snd/accept.wav");
    s_context.sounds.load(SoundID::NUI_REFUSE, "res/snd/refuse.wav");
    s_context.sounds.load(SoundID::NUI_SELECT, "res/snd/select.wav");

    refreshSounds();
}

void Application::refreshSounds()
{
    const auto& resolution = s_context.windowInfo.resolution;
    const auto& effectiveSoundVolume = s_context.sound.effectiveSoundVolume;

    s_context.sounds.setListenerPosition(resolution / 2.f);
    s_context.sounds.setVolume(effectiveSoundVolume);
}
