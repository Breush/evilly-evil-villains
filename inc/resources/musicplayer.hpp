#pragma once

#include "tools/int.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Audio/Music.hpp>

#include <map>
#include <string>

// Forward declaration
enum class MusicID : uint8;

namespace resources
{
    class MusicPlayer : private sf::NonCopyable
    {
    public:
        MusicPlayer();

        void load(MusicID id, const std::string& filename);

        void play(MusicID id);
        void stop();

        void setPaused(bool paused);
        void setVolume(uint volume);

    private:
        sf::Music m_music;
        std::map<MusicID, std::string> m_musics;
        uint m_volume;
    };
}
