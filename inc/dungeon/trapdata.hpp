#pragma once

#include "tools/metadata.hpp"
#include "tools/tools.hpp"
#include "tools/debug.hpp"
#include "tools/platform-fixes.hpp" // find_if

#include <pugixml/pugixml.hpp>
#include <unordered_map>
#include <string>

namespace dungeon
{
    /*!
     *  A dynamic structure holding traps information
     *  and which can be exported and imported as XML.
     */

    class TrapData final
    {
    public:

        //! Default constructor.
        TrapData() = default;

        //! Default destructor.
        ~TrapData() = default;

        //---------------------//
        //! @name Manipulation
        //! @{

        //! Remove all attributes and trap no longer exists.
        void clear();

        //! Initialize a empty TrapData of given type.
        void create(std::wstring type);

        //! Returns true if the trap has no attributes.
        inline bool empty() const { return m_attributes.empty(); }

        //! @}

        //------------------------//
        //! @name XML interaction
        //! @{

        //! Save data to the node.
        void saveXML(pugi::xml_node& node);

        //! Load data from the node.
        void loadXML(const pugi::xml_node& node);

        //! @}

        //-------------//
        //! @name Cost
        //! @{

        //! How much the trap cost on creation.
        uint onCreateCost() const;

        //! How much the trap cost on destruction.
        uint onDestroyGain() const;

        //! @}

        //----------------//
        //! @name Getters
        //! @{

        //! Access an attribute.
        inline MetaData& operator[](const std::wstring& name) { return m_attributes[name]; }

        //! Whether the trap exists (false if cleared or just created).
        inline bool exists() const { return m_exists; }

        //! The type of trap.
        inline const std::wstring& type() const { return m_type; }

        //! @}

    private:

        bool m_exists = false;  //!< Whether the trap exists or not.
        std::wstring m_type;    //!< The type of trap.
        std::unordered_map<std::wstring, MetaData> m_attributes; //!< The attributes.
    };

    //------------------------------//
    //----- External functions -----//

    //! Returns true if we found an existing TrapData of specified type.
    template<class Container>
    inline bool hasOfType(const Container& container, const std::wstring& type)
    {
        for (const auto& data : container)
            if (data.exists() && data.type() == type)
                return true;
        return false;
    }

    //! Returns if found an existing TrapData iterator of specified type, or std::end(container) else.
    template<class Container>
    inline auto findOfType(Container& container, const std::wstring& type) -> decltype(std::end(container))
    {
        return std::find_if(container, [&](const TrapData& data) { return data.exists() && data.type() == type; });
    }
}

