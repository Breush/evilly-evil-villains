#pragma once

#include "resources/holder.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>

#include <list>

namespace resources
{
    //! Holder for sounds.
    // TODO OPTIM Make a sound pool?

    class SoundPlayer final : private sf::NonCopyable
    {
    public:

        //! Constructor.
        SoundPlayer();

        //! Default destructor.
        ~SoundPlayer() = default;

        //---------------------------//
        //! @name Storage and access
        //! @{

        //! Load resource into memory.
        void load(const std::string& filename);

        //! Gets the ID of the resource from filename.
        std::string getID(const std::string& filename);

        //! Remove all resource from memory with id starting with specified prefix.
        void freeMatchingPrefix(const std::string& prefix);

        //! @}

        //----------------------//
        //! @name Sound control
        //! @{

        //! Play a sound, from default position.
        void play(const std::string& id);

        //! Play a sound, from specified position.
        void play(const std::string& id, sf::Vector2f position);

        //! Pause or resume all sounds.
        void setPaused(bool paused);

        //! Stop all sounds.
        //! Note: not removing them from list.
        void stopAll();

        //! Set the volume to play new sounds.
        void setVolume(float volume);

        //! Clean list of sounds.
        void removeStoppedSounds();

        //! @}

        //-----------------//
        //! @name Listener
        //! @{

        //! Set the default position for new sounds.
        void setListenerPosition(sf::Vector2f position);

        //! Get the default position for new sounds.
        sf::Vector2f getListenerPosition() const;

        //! @}

    private:

        SoundBufferHolder m_soundBuffers;                   //!< Sound player.
        std::list<std::unique_ptr<sf::Sound>> m_sounds;     //!< Sound list.
        float m_volume = 100.f;                             //!< Current volume for new sounds.


        const float m_listenerZ = 300.f;        //!< Z-depth for listener.
        const float m_minDistance2D = 200.f;    //!< Distance from screen.
        const float m_minDistance3D = 360.f;    //!< sqrt(minDistance2D^2 + m_listener2^2)
        const float m_attenuation = 50.f;       //!< Attenuation factor.
    };
}
