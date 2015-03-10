#include "resources/musicplayer.hpp"

#include "resources/identifiers.hpp"
#include "tools/debug.hpp"

#include <stdexcept> // runtime_error

using namespace resources;

MusicPlayer::MusicPlayer()
    : m_music()
    , m_volume(100)
{
}

void MusicPlayer::load(MusicID id, const std::string& filename)
{
    auto inserted = m_musics.insert(std::make_pair(id, filename));
    massert(inserted.second, "Cannot add music filename.");
}

void MusicPlayer::play(MusicID id)
{
    auto found = m_musics.find(id);
    massert(found != m_musics.end(), "MusicID " << static_cast<uint8>(id) << " was not previously loaded.");

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
