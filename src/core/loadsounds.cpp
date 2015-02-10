#include "core/application.hpp"

void Application::loadSounds()
{
    // Splash-screen
    s_context.sounds.load(Sounds::JUMPINGTOASTS, "res/snd/jumping-toasts.wav", true);

    // NUI
    s_context.sounds.load(Sounds::NUI_ACCEPT, "res/snd/accept.wav");
    s_context.sounds.load(Sounds::NUI_REFUSE, "res/snd/refuse.wav");
    s_context.sounds.load(Sounds::NUI_SELECT, "res/snd/select.wav");
}
