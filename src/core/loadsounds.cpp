#include "core/application.hpp"

#include "core/debug.hpp"
#include "context/context.hpp"
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

        context::context.sounds.load(fileInfo.fullName);

        ++soundsCount;
    }

    mdebug_core_2("Loaded " << soundsCount << " sounds.");

    refreshSounds();
}

void Application::refreshSounds()
{
    const auto& resolution = context::context.windowInfo.resolution;
    const auto& effectiveSoundVolume = context::context.audio.effectiveSoundVolume;

    context::context.sounds.setListenerPosition(resolution / 2.f);
    context::context.sounds.setVolume(effectiveSoundVolume);
}

void Application::updateSounds(const sf::Time& dt)
{
    static sf::Time soundsTime; // Set to zero according to SFML API
    soundsTime += dt;

    // Clean all sounds regularly
    if (soundsTime >= m_soundsRefreshTime) {
        context::context.sounds.removeStoppedSounds();
        soundsTime -= m_soundsRefreshTime;
    }
}
