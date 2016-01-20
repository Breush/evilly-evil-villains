#include "core/application.hpp"

#include "core/debug.hpp"
#include "tools/filesystem.hpp"

// FIXME Should be on the same structure that textures and animations,
// as more and more sounds will be loaded

void Application::loadSounds()
{
    uint soundsCount = 0u;

    // Recursively load all files in resource directory
    for (const auto& fileInfo : listFiles("res", true)) {
        // Load only sounds files
        if (fileInfo.isDirectory || fileExtension(fileInfo.name) != "wav")
            continue;

        s_context.sounds.load(fileInfo.fullName);

        ++soundsCount;
    }

    mdebug_core_2("Loaded " << soundsCount << " sounds.");

    refreshSounds();
}

void Application::refreshSounds()
{
    const auto& resolution = s_context.windowInfo.resolution;
    const auto& effectiveSoundVolume = s_context.audio.effectiveSoundVolume;

    s_context.sounds.setListenerPosition(resolution / 2.f);
    s_context.sounds.setVolume(effectiveSoundVolume);
}

void Application::updateSounds(const sf::Time& dt)
{
    static sf::Time soundsTime; // Set to zero according to SFML API
    soundsTime += dt;

    // Clean all sounds regularly
    if (soundsTime >= m_soundsRefreshTime) {
        s_context.sounds.removeStoppedSounds();
        soundsTime -= m_soundsRefreshTime;
    }
}
