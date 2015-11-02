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
    //! All common info for each monster and should never change.

    class MonstersDB final
    {
    public:

        // TODO That struct could be external and reused for TrapsDB.

        //! What ressources a cost is equivalent.
        struct Cost
        {
            uint dosh = 0u; //!< Dosh.
            uint soul = 0u; //!< Soul.
            uint fame = 0u; //!< Fame.
        };

        //! All infos that we can find in each monster.
        struct MonsterData
        {
            std::wstring name = L"(Unknown)";   //!< Translated name. TODO: How to get translation for this?
            Cost baseCost;                      //!< Starting price.
            Cost monthlyCost;                   //!< Salary.
            sf::Vector2f speed;                 //!< Speed relative to dungeon room size.
            float pauseDelay;                   //!< How many seconds to stay still if the monster stays in the room.
        };

    public:

        //! Constructor.
        MonstersDB();

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
