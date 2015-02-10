#pragma once

#include "tools/int.hpp"
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

    void load(Musics::ID id, const std::string& filename);

    void play(Musics::ID id);
    void stop();

    void setPaused(bool paused);
    void setVolume(uint volume);

private:
    sf::Music m_music;
    std::map<Musics::ID, std::string> m_musics;
    uint m_volume;
};

