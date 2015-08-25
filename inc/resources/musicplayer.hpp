#pragma once

#include "tools/int.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Audio/Music.hpp>

#include <string>

namespace resources
{
    //! Holder for musics.

    class MusicPlayer final : private sf::NonCopyable
    {
    public:

        //! Default constructor.
        MusicPlayer() = default;

        //! Default destructor.
        ~MusicPlayer() = default;

        //----------------------//
        //! @name Music control
        //! @{

        //! Start a specific music stored in res/mus/ folder.
        void play(const std::string& id);

        //! Stop streaming the current music if any, and free the memory.
        void stop();

        //! Pause or unpause the current music if any.
        void setPaused(bool paused);

        //! Set the volume to the current and following musics.
        //! Volume is a floating number between 0.f and 100.f.
        void setVolume(float volume);

        //! @}

    private:

        //! The music.
        sf::Music m_music;

        //! The volume.
        float m_volume;
    };
}
