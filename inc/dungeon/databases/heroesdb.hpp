#pragma once

#include "tools/int.hpp"

#include <string>
#include <unordered_map>

#include <SFML/System/Vector2.hpp>

// Forward declarations

namespace pugi
{
    class xml_node;
}

namespace dungeon
{
    //! All common info for each hero that should never change.

    class HeroesDB final
    {
    public:

        //! All infos that we can find in each hero.
        struct HeroData
        {
            std::wstring name = L"(Unknown)";   //!< Translated name.
            float startingHP = 1.f;             //!< Health points when the hero is created.
            sf::Vector2f speed;                 //!< Speed relative to dungeon room size.
            float pauseDelay = 0.f;             //!< How many seconds to stay still if the hero stays in the room.
        };

    public:

        //! Constructor.
        HeroesDB();

        //---------------------//
        //! @name Data control
        //! @{

        //! Force reload of data files.
        void reload();

        //! @}

        //------------------//
        //! @name Accessors
        //! @{

        //! Get the data associated to a hero.
        inline const HeroData& get(const std::wstring& id) const { return m_heroesData.at(id); }

        //! Get all the possible heroes.
        inline const std::unordered_map<std::wstring, HeroData>& get() const { return m_heroesData; }

        //! @}

    protected:

        //---------------------//
        //! @name Data control
        //! @{

        //! Load the xml file and add its data to the map.
        void add(const std::string& filename);

        //! Read node and affect it to a speed variable.
        void readSpeedNode(sf::Vector2f& speed, const pugi::xml_node& node);

        //! @}

    private:

        std::unordered_map<std::wstring, HeroData> m_heroesData;    //!< All heroes data.
    };
}
