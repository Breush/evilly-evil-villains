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
    //! All common info for each facility and should never change.

    class FacilitiesDB final
    {
    public:

        //! All infos that we can find in each facility.
        struct FacilityData
        {
            std::wstring name = L"(Unknown)";   //!< Translated name.
            Cost baseCost;                      //!< Construction price.
        };

    public:

        //! Constructor.
        FacilitiesDB();

        //---------------------//
        //! @name Data control
        //! @{

        //! Force reload of data files.
        void reload();

        //! @}

        //------------------//
        //! @name Accessors
        //! @{

        //! Get the data associated.
        inline const FacilityData& get(const std::wstring& id) const { return m_facilitiesData.at(id); }

        //! Get all the possibilities.
        inline const std::unordered_map<std::wstring, FacilityData>& get() const { return m_facilitiesData; }

        //! @}

    protected:

        //---------------------//
        //! @name Data control
        //! @{

        //! Load the xml file and add its data to the map.
        void add(const std::string& filename);

        //! Read node and affect it to a cost variable.
        void readCostNode(Cost& cost, const pugi::xml_node& node);

        //! @}

    private:

        std::unordered_map<std::wstring, FacilityData> m_facilitiesData;    //!< All data.
    };
}
