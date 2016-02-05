#include "core/application.hpp"

#include "core/debug.hpp"
#include "context/context.hpp"
#include "tools/filesystem.hpp"

void Application::loadSounds(const std::initializer_list<std::string>& folders)
{
    // Recursively load all files in resource directory
    for (const auto& folder : folders) {
        uint soundsCount = 0u;

        for (const auto& fileInfo : listFiles("res/" + folder, true)) {
            // Load only wav files
            if (fileInfo.isDirectory || fileExtension(fileInfo.name) != "wav")
                continue;

            context::context.sounds.load(fileInfo.fullName);

            ++soundsCount;
        }

        mdebug_core_2("Loaded " << soundsCount << " sounds from " << folder << ".");
    }

    refreshSounds();
}

void Application::freeSounds(const std::initializer_list<std::string>& folders)
{
    for (const auto& folder : folders) {
        context::context.sounds.freeMatchingPrefix(folder);
        mdebug_core_2("Freed sounds from " << folder << ".");
    }
}

void Application::preloadSounds()
{
    loadSounds({"core"});
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
