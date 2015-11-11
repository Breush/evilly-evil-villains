#pragma once

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
    //! All common info for each trap and should never change.

    class TrapsDB final
    {
    public:

        //! How strong an object is.
        struct Resistance
        {
            uint durability = 0u;   //!< Health point of the trap.
        };

        //! All infos that we can find in each trap.
        struct TrapData
        {
            std::wstring name = L"(Unknown)";   //!< Translated name.
            Cost baseCost;                      //!< Construction price.
            Resistance resistance;              //!< How much a trap can resist.
            RelCost repairCost;                 //!< Relative to damage repair cost.
        };

    public:

        //! Constructor.
        TrapsDB();

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
        inline const TrapData& get(const std::wstring& id) const { return m_trapsData.at(id); }

        //! Get all the possible monsters.
        inline const std::unordered_map<std::wstring, TrapData>& get() const { return m_trapsData; }

        //! @}

    protected:

        //---------------------//
        //! @name Data control
        //! @{

        //! Load the xml file and add its data to the map.
        void add(const std::string& filename);

        //! Read node and affect it to a cost variable.
        void readCostNode(Cost& cost, const pugi::xml_node& node);

        //! Read node and affect it to a relative cost variable.
        void readRelCostNode(RelCost& cost, const pugi::xml_node& node);

        //! Read node and affect it to a resistance variable.
        void readResistanceNode(Resistance& speed, const pugi::xml_node& node);

        //! @}

    private:

        std::unordered_map<std::wstring, TrapData> m_trapsData; //!< All traps data.
    };
}
