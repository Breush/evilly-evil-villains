#include "resources/soundplayer.hpp"

#include <SFML/Audio/Listener.hpp>

#include <cmath>

namespace
{
    // Sound coordinate system, point of view of a player in front of the screen:
    // X = left; Y = up; Z = back (out of the screen)
    const float ListenerZ = 300.f;
    const float Attenuation = 8.f;
    const float MinDistance2D = 200.f;
    const float MinDistance3D = std::sqrt(MinDistance2D*MinDistance2D + ListenerZ*ListenerZ);
}

SoundPlayer::SoundPlayer()
    : m_soundBuffers()
    , m_sounds()
{
    // Listener points towards the screen (default in SFML)
    sf::Listener::setDirection(0.f, 0.f, -1.f);
}

void SoundPlayer::load(Sounds::ID id, const std::string& filename)
{
    m_soundBuffers.load(id, filename);
}

Sounds::ID SoundPlayer::getID(const std::string& filename)
{
    m_soundBuffers.getID(filename);
}

void SoundPlayer::play(Sounds::ID id)
{
    play(id, getListenerPosition());
}

void SoundPlayer::play(Sounds::ID id, sf::Vector2f position)
{
    m_sounds.push_back(sf::Sound());
    sf::Sound& sound = m_sounds.back();

    sound.setBuffer(m_soundBuffers.get(id));
    sound.setPosition(position.x, -position.y, 0.f);
    sound.setAttenuation(Attenuation);
    sound.setMinDistance(MinDistance3D);

    sound.play();
}

void SoundPlayer::removeStoppedSounds()
{
    m_sounds.remove_if([](const sf::Sound& s) {
        return s.getStatus() == sf::Sound::Stopped;
    });
}

void SoundPlayer::setListenerPosition(sf::Vector2f position)
{
    sf::Listener::setPosition(position.x, -position.y, ListenerZ);
}

sf::Vector2f SoundPlayer::getListenerPosition() const
{
    sf::Vector3f position = sf::Listener::getPosition();
    return sf::Vector2f(position.x, -position.y);
}
