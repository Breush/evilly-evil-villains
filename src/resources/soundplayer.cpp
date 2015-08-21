#include "resources/soundplayer.hpp"

#include "tools/platform-fixes.hpp" // erase_if
#include "core/gettext.hpp" // string2wstring

#include <SFML/Audio/Listener.hpp>
#include <cmath>

using namespace resources;

SoundPlayer::SoundPlayer()
    : m_soundBuffers()
    , m_sounds()
{
    // Listener points towards the screen (default in SFML)
    sf::Listener::setDirection(0.f, 0.f, -1.f);
}

void SoundPlayer::load(const std::string& filename)
{
    m_soundBuffers.load(filename);
}

void SoundPlayer::play(const std::string& id)
{
    play(id, getListenerPosition());
}

void SoundPlayer::play(const std::string& id, sf::Vector2f position)
{
    m_sounds.emplace_back();
    sf::Sound& sound = m_sounds.back();

    sound.setBuffer(m_soundBuffers.get(id));
    sound.setPosition(position.x, -position.y, 0.f);
    sound.setAttenuation(m_attenuation);
    sound.setMinDistance(m_minDistance3D);
    sound.setVolume(m_volume);

    sound.play();
}

void SoundPlayer::stopAll()
{
    for (auto& sound : m_sounds)
        sound.stop();
}

void SoundPlayer::removeStoppedSounds()
{
    std::erase_if(m_sounds, [](const sf::Sound& s) { return s.getStatus() == sf::Sound::Stopped; });
}

void SoundPlayer::setVolume(float volume)
{
    m_volume = volume;
}

void SoundPlayer::setListenerPosition(sf::Vector2f position)
{
    sf::Listener::setPosition(position.x, -position.y, m_listenerZ);
}

sf::Vector2f SoundPlayer::getListenerPosition() const
{
    sf::Vector3f position = sf::Listener::getPosition();
    return sf::Vector2f(position.x, -position.y);
}
