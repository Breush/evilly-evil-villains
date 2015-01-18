#pragma once

#include "resources/holder.hpp"
#include "resources/identifiers.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Audio/Music.hpp>

#include <map>
#include <string>


class MusicPlayer : private sf::NonCopyable
{
public:
    MusicPlayer();

    void play(Music::ID theme);
    void stop();

    void setPaused(bool paused);
    void setVolume(uint volume);


private:
    sf::Music m_music;
    std::map<Music::ID, std::string> m_filenames;
    uint m_volume;
};

