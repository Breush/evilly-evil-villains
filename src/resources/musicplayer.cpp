#include "resources/musicplayer.hpp"

#include "tools/debug.hpp"
#include "tools/tools.hpp"

#include <stdexcept> // runtime_error

using namespace resources;

//-------------------------//
//----- Music control -----//

void MusicPlayer::play(const std::string& id)
{
    if (!m_music.openFromFile("res/" + id + ".ogg"))
        mquit("Music '" + id + "' could not be loaded.");

    m_currentID = id;
    m_music.setVolume(m_volume);
    m_music.setLoop(true);
    m_music.play();
}

void MusicPlayer::stop(const std::string& id)
{
    if (id.empty() || m_currentID == id) {
        m_currentID = "";
        m_music.stop();
    }
}

void MusicPlayer::setVolume(float volume)
{
    returnif (m_volume == volume);
    m_volume = volume;
    m_music.setVolume(m_volume);
}

void MusicPlayer::setPaused(bool paused)
{
    // Do not change stopped music
    if (m_music.getStatus() != sf::Music::Stopped)
        (paused)? m_music.pause() : m_music.play();
}
