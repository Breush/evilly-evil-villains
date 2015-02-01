#pragma once

#include "resources/holder.hpp"
#include "resources/identifiers.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>

#include <list>

class SoundPlayer : private sf::NonCopyable
{
public:
    SoundPlayer();

    void load(Sounds::ID id, const std::string& filename);
    Sounds::ID getID(const std::string& filename);

    void play(Sounds::ID id);
    void play(Sounds::ID id, sf::Vector2f position);

    void stopAll();

    void removeStoppedSounds();
    void setListenerPosition(sf::Vector2f position);
    sf::Vector2f getListenerPosition() const;

private:
    SoundBufferHolder m_soundBuffers;
    std::list<sf::Sound> m_sounds;
};

