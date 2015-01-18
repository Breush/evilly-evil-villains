#include "resources/musicplayer.hpp"


MusicPlayer::MusicPlayer()
    : m_music()
    , m_filenames()
    , m_volume(100)
{
    m_filenames[Music::MENU_THEME] = "res/mus/menu_theme.ogg";
}

void MusicPlayer::play(Music::ID theme)
{
    std::string filename = m_filenames[theme];

    if (!m_music.openFromFile(filename))
        throw std::runtime_error("Music " + filename + " could not be loaded.");

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
