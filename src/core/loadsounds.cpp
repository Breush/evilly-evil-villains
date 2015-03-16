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
}