#pragma once

#include "tools/int.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Audio/Music.hpp>

#include <vector>
#include <string>

// TODO DOC

namespace resources
{
    class MusicPlayer : private sf::NonCopyable
    {
    public:
        MusicPlayer() = default;

        void play(const std::string&  id);
        void stop();

        void setPaused(bool paused);
        void setVolume(float volume);

    private:

        sf::Music m_music;
        float m_volume;
    };
}
