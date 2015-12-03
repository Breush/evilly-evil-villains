#pragma once

#include "tools/int.hpp"
#include "context/cost.hpp"

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
    //! All common info for each monster and should never change.

    class MonstersDB final
    {
    public:

        //! All infos that we can find in each monster.
        struct MonsterData
        {
            std::wstring name = L"(Unknown)";   //!< Translated name.
            Cost baseCost;                      //!< Starting price.
            Cost weeklyCost;                    //!< Salary.
            sf::Vector2f speed;                 //!< Speed relative to dungeon room size.
            float pauseDelay = 0.f;             //!< How many seconds to stay still if the monster stays in the room.
            uint hireCountdown = 0u;            //!< How many seconds to wait after an hire occurs.
        };

    public:

        //! Constructor.
        MonstersDB();

        //---------------------//
        //! @name Data control
        //! @{

        //! Force reload of data files.
        void reload();

        //! @}

        //------------------//
        //! @name Accessors
        //! @{

        //! Get the data associated to a monster.
        inline const MonsterData& get(const std::wstring& id) const { return m_monstersData.at(id); }

        //! Get all the possible monsters.
        inline const std::unordered_map<std::wstring, MonsterData>& get() const { return m_monstersData; }

        //! @}

    protected:

        //---------------------//
        //! @name Data control
        //! @{

        //! Load the xml file and add its data to the map.
        void add(const std::string& filename);

        //! Read node and affect it to a cost variable.
        void readCostNode(Cost& cost, const pugi::xml_node& node);

        //! Read node and affect it to a speed variable.
        void readSpeedNode(sf::Vector2f& speed, const pugi::xml_node& node);

        //! @}

    private:

        std::unordered_map<std::wstring, MonsterData> m_monstersData;   //!< All monsters data.
    };
}
