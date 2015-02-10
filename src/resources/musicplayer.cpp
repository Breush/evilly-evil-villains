#include "resources/musicplayer.hpp"

MusicPlayer::MusicPlayer()
    : m_music()
    , m_volume(100)
{
}

void MusicPlayer::load(Musics::ID id, const std::string& filename)
{
    auto inserted = m_musics.insert(std::make_pair(id, filename));
    massert(inserted.second, "Cannot add music filename.");
}

void MusicPlayer::play(Musics::ID id)
{
    auto found = m_musics.find(id);
    massert(found != m_musics.end(), "Musics::ID " << id << " was not previously loaded.");

    if (!m_music.openFromFile(found->second))
        throw std::runtime_error("Music " + found->second + " could not be loaded.");

    m_music.setVolume(m_volume);
    m_music.setLoop(true);
    m_music.play();
}

void MusicPlayer::stop()
{
    m_music.stop();
}

void MusicPlayer::setVolume(uint volume)
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
