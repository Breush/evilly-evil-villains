#pragma once

#include "tools/int.hpp"

#include <string>
#include <vector>
#include <ctime>

#include <SFML/System/NonCopyable.hpp>

namespace context
{
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

        //! Add a world in memory (nothing saved yet) and create empty folder.
        //! @return The index of the new world.
        uint add(std::wstring name, std::wstring villain);

        //! Find and remove all worlds associated with a villain.
        void remove(const std::wstring& villain);

        //! Find how many worlds are associated with the specified villain.
        uint count(const std::wstring& villain);

        //! Refresh the lastPlayer attribute to current time of the selected world.
        void refreshLastPlayed();

        //! @}

        //----------------//
        //! @name Setters
        //! @{

        //! Get the vector of all worlds for read.
        inline const World& select(uint index) { return *(m_selected = &get(index)); }

        //! @}

        //----------------//
        //! @name Getters
        //! @{

        //! Get the current world information.
        inline const World& selected() const { return *m_selected; }

        //! Get the vector of all worlds for read.
        inline const std::vector<World>& get() const { return m_worlds; }

        //! Get a specific world for read.
        inline const World& get(uint index) const { return m_worlds[index]; }

        //! @}

    private:

        std::vector<World> m_worlds;        //!< The vector of all worlds.
        const World* m_selected = nullptr;  //!< The currently selected world.
    };

    //! The static member from worlds context.
    extern Worlds worlds;
}

