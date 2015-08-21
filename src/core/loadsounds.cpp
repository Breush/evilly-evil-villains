#include "core/application.hpp"

#include "core/debug.hpp"
#include "tools/filesystem.hpp"

void Application::loadSounds()
{
    uint soundsCount = 0u;

    // Recursively load all files in resource directory
    for (const auto& fileInfo : listFiles("res/snd", true)) {
        // Load only sounds files
        if (fileInfo.isDirectory || fileExtension(fileInfo.name) != "wav")
            continue;

        s_context.sounds.load(fileInfo.fullName);

        ++soundsCount;
    }

    mdebug_core_1("Loaded " << soundsCount << " sounds.");

    refreshSounds();
}

void Application::refreshSounds()
{
    const auto& resolution = s_context.windowInfo.resolution;
    const auto& effectiveSoundVolume = s_context.sound.effectiveSoundVolume;

    s_context.sounds.setListenerPosition(resolution / 2.f);
    s_context.sounds.setVolume(effectiveSoundVolume);
}
