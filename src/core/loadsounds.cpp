#include "core/application.hpp"

#include "resources/identifiers.hpp"

void Application::loadSounds()
{
    // TODO Should be in refreshSomething
    s_context.sounds.setListenerPosition(s_context.resolution / 2.f);
    s_context.sounds.setVolume(s_context.sound.effectiveSoundVolume);

    // Splash-screen
    s_context.sounds.load(SoundID::JUMPINGTOASTS, "res/snd/jumping-toasts.wav", true);

    // NUI
    s_context.sounds.load(SoundID::NUI_ACCEPT, "res/snd/accept.wav");
    s_context.sounds.load(SoundID::NUI_REFUSE, "res/snd/refuse.wav");
    s_context.sounds.load(SoundID::NUI_SELECT, "res/snd/select.wav");
}
