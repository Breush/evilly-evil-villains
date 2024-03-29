#pragma once

#include "context/cost.hpp"
#include "dungeon/databases/link.hpp"
#include "dungeon/structs/roomflag.hpp"

#include <vector>
#include <unordered_map>

#include <SFML/System/Vector2.hpp>

// Forward declarations

namespace pugi
{
    class xml_node;
    class xml_attribute;
}

namespace dungeon
{
    //! All infos that we can find in each facility.
    struct FacilityData
    {
        std::wstring name = L"(Unknown)";               //!< Translated name.

        Cost baseCost;                                  //!< Construction price.

        std::vector<FixedLink> fixedLinks;              //!< All the links that do not require player's intervention.
        std::vector<InteractiveLink> interactiveLinks;  //!< All the links that require player's intervention to position.
        std::vector<Warning> warnings;                  //!< All the warnings to send.
        std::vector<Constraint> constraints;            //!< Construction constraints (absolute).

        bool listed = true;                             //!< Is the facility constructible by the player?
        bool hitbox = true;                             //!< Does the facility have a hitbox?
        bool entrance = false;                          //!< Does the facility provide an entry point to the dungeon.
        bool energetic = false;                         //!< Whether the facility can be controlled with energy.
        bool permissive = false;                        //!< Can we destroy this facility to make space for others?

        uint8 lock = RoomFlag::NONE;                    //!< What part of the room the facility blocks for access.
        uint8 hide = RoomFlag::NONE;                    //!< What part of the room the facility hides.

        //! Find a link (fixed or interactive) from its ID.
        inline const Link* linkFind(uint linkID) const
        {
            for (const auto& link : fixedLinks)
                if (link.id == linkID)
                    return &link;

            for (const auto& link : interactiveLinks)
                if (link.id == linkID)
                    return &link;

            return nullptr;
        }
    };

    //! All common info for each facility and should never change.

    class FacilitiesDB final
    {
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

        //! Get how many listed facilities there are.
        inline uint listedCount() const { return m_listedCount; }

        //! @}

    protected:

        //---------------------//
        //! @name Data control
        //! @{

        //! Load the xml file and add its data to the map.
        void add(const std::string& filename);

        //! Read node and affect it to a link variable (fixed or interactive).
        void readLinkNode(Link& link, const pugi::xml_node& node);

        //! Read attribute and affect it to a lock variable.
        void readRoomFlagsAttribute(uint8& lock, const pugi::xml_attribute& attribute);

        //! Read node and affect it to a cost variable.
        void readCostNode(Cost& cost, const pugi::xml_node& node);

        //! Read node and affect it to a constraint variable.
        void readConstraintNode(Constraint& constraint, const pugi::xml_node& node);

        //! Read attribute and affect it to a constraint parameter variable.
        void readConstraintParameterAttribute(ConstraintParameter& constraintParameter, const pugi::xml_attribute& attribute);

        //! @}

    private:

        std::unordered_map<std::wstring, FacilityData> m_facilitiesData;    //!< All data.
        uint m_listedCount = 0u;                                            //!< How many listed facilities there are.
    };
}
