#include "resources/musicplayer.hpp"

#include "tools/debug.hpp"

#include <stdexcept> // runtime_error

using namespace resources;

void MusicPlayer::play(const std::string& id)
{
    if (!m_music.openFromFile("res/mus/" + id + ".ogg"))
        throw std::runtime_error("Music '" + id + "' could not be loaded.");

    m_music.setVolume(m_volume);
    m_music.setLoop(true);
    m_music.play();
}

void MusicPlayer::stop()
{
    m_music.stop();
}

void MusicPlayer::setVolume(float volume)
{
    if (m_volume != volume) {
        m_volume = volume;
        m_music.setVolume(m_volume);
    }
}

void MusicPlayer::setPaused(bool paused)
{
    if (paused)
        m_music.pause();
    else
        m_music.play();
}
