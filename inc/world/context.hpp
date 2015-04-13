#pragma once

#include "tools/int.hpp"

#include <string>
#include <vector>
#include <ctime>

#include <SFML/System/NonCopyable.hpp>

namespace world
{
    //! A XML loading class to get the worlds data.
    /*!
     *  This class allow you to select a world within
     *  the list coming from the default file saves/worlds.xml.
     */

    class Context final : sf::NonCopyable
    {
    public:

        //! Defines world attributes.
        struct World
        {
            uint index;                 //!< The index of the world in the worlds vector.
            std::wstring name;          //!< The name of the world.
            std::wstring villain;       //!< The villain on this world.
            uint dungeons;              //!< The number of dungeons in the world.
            std::wstring mainDungeon;   //!< The name of the biggest of all dungeons on the world.
            time_t created;             //!< The timestamp of creation of the world.
            time_t lastPlayed;          //!< The timestamp of last time user played on.
            std::wstring folder;        //!< The sub-folder in which the world is saved.
        };

    public:

        //! Default constructor.
        Context() = default;

        //! Default destructor.
        ~Context() = default;

        //------------------------//
        //! @name File management
        //! @{

        //! Reload all the worlds into memory.
        void load();

        //! Save all the worlds to the default file.
        void save();

        //! @}

        //--------------------//
        //! @name Interaction
        //! @{

        //! Add a world in memory (nothing saved) and create empty folder.
        //! @return The index of the new world.
        uint createWorld(std::wstring worldName, std::wstring villain);

        //! Refresh the lastPlayer attribute to current time of the selected world.
        void refreshLastPlayed();

        //! @}

        //----------------//
        //! @name Setters
        //! @{

        //! Get the vector of all worlds for read.
        inline const World& selectWorld(uint worldIndex) { return *(m_selectedWorld = &world(worldIndex)); }

        //! @}

        //----------------//
        //! @name Getters
        //! @{

        //! Get the current world information.
        inline const World& selectedWorld() const { return *m_selectedWorld; }

        //! Get the vector of all worlds for read.
        inline const std::vector<World>& worlds() const { return m_worlds; }

        //! Get a specific world for read.
        inline const World& world(uint worldIndex) const { return m_worlds[worldIndex]; }

        //! @}

    private:

        //! The vector of all worlds.
        std::vector<World> m_worlds;

        //!< The current selected world.
        const World* m_selectedWorld = nullptr;
    };

    //! The static member from worlds context.
    extern Context context;
}

