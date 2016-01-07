#pragma once

#include "tools/int.hpp"

#include <string>
#include <vector>
#include <ctime>

#include <SFML/System/NonCopyable.hpp>

namespace context
{
    //! Types of available game modes.
    enum Gamemode
    {
        UNKNOWN     = 0u,
        STORY       = 1u,
        RICHMAN     = 2u,
        HARDCORE    = 3u,
        COUNT       = 4u,      //!< Keep last.
    };

    //! A XML loading class to get the worlds data.
    /*!
     *  This class allow you to select a world within
     *  the list coming from the default file saves/worlds.xml.
     */

    class Worlds final : sf::NonCopyable
    {
    public:

        //! Defines world attributes.
        struct World
        {
            uint index;                             //!< The index of the world in the worlds vector.
            std::wstring name;                      //!< The name of the world.
            std::wstring villain;                   //!< The villain on this world.
            Gamemode gamemode = Gamemode::UNKNOWN;  //!< The currently played gamemode.
            time_t created;                         //!< The timestamp of creation of the world.
            time_t lastPlayed;                      //!< The timestamp of last time user played on.
            std::wstring folder;                    //!< The sub-folder in which the world is saved.
        };

    public:

        //! Default constructor.
        Worlds() = default;

        //! Default destructor.
        ~Worlds() = default;

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

        //! Add a world in memory (nothing saved yet).
        //! One should use setNameCreation afterwards to not save an invalid world.
        //! @return The index of the new world.
        uint add();

        //! Set the world name and create empty folder.
        void setNameCreation(World& world, std::wstring name);

        //! Find and remove the world.
        void remove(const World& world);

        //! Find and remove the world of the specified folder.
        void removeFromFolder(const std::wstring& folder);

        //! Find and remove all worlds associated with a villain.
        void removeFromVillain(const std::wstring& villain);

        //! Find how many worlds are associated with the specified villain.
        uint countFromVillain(const std::wstring& villain);

        //! Refresh the lastPlayer attribute to current time of the selected world.
        void refreshLastPlayed();

        //! @}

        //----------------------------//
        //! @name Setters and getters
        //! @{

        //! Get the vector of all worlds for read.
        World& select(uint index);

        //! Get the current world information.
        inline World& selected() { return *m_selected; }

        //! Get the current world information (const).
        inline const World& selected() const { return *m_selected; }

        //! Get the vector of all worlds for read.
        inline const std::vector<World>& get() const { return m_worlds; }

        //! Get a specific world for read.
        inline const World& get(uint index) const { return m_worlds[index]; }

        //! @}

    private:

        std::vector<World> m_worlds;    //!< The vector of all worlds.
        World* m_selected = nullptr;    //!< The currently selected world.
    };

    //! The static member from worlds context.
    extern Worlds worlds;
}

