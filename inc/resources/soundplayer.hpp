#pragma once

#include "resources/holder.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>

#include <list>

namespace resources
{
    class SoundPlayer : private sf::NonCopyable
    {
    public:
        SoundPlayer();

        void load(SoundID id, const std::string& filename, bool store = false);

        void storeID(SoundID id, const std::wstring& filename);
        SoundID getID(const std::wstring& filename);

        void play(SoundID id);
        void play(SoundID id, sf::Vector2f position);

        void stopAll();
        void setVolume(float volume);

        void removeStoppedSounds();
        void setListenerPosition(sf::Vector2f position);
        sf::Vector2f getListenerPosition() const;

    private:
        resources::SoundBufferHolder m_soundBuffers;
        std::list<sf::Sound> m_sounds;

        float m_volume = 100.f;
        const float m_listenerZ = 300.f;
        const float m_minDistance3D = 360.f; // sqrt(minDistance2D^2 + m_listener2^2)
        const float m_attenuation = 50.f;
        const float m_minDistance2D = 200.f;
    };
}

