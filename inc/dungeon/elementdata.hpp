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
     *  A dynamic structure holding dungeon element information
     *  and which can be exported and imported as XML.
     */

    class ElementData final
    {
    public:

        //! Default constructor.
        ElementData() = default;

        //! Default destructor.
        ~ElementData() = default;

        //---------------------//
        //! @name Manipulation
        //! @{

        //! Remove all attributes and data no longer exists.
        void clear();

        //! Initialize a empty ElementData of given type.
        void create(std::wstring type);

        //! Returns true if the data has no attributes.
        inline bool empty() const { return m_attributes.empty(); }

        //! @}

        //------------------------//
        //! @name XML interaction
        //! @{

        //! Save data to the node.
        void saveXML(pugi::xml_node& node) const;

        //! Load data from the node.
        void loadXML(const pugi::xml_node& node);

        //! @}

        //----------------//
        //! @name Getters
        //! @{

        //! Access an existing attribute (const).
        inline const MetaData& at(const std::wstring& name) const { return m_attributes.at(name); }

        //! Access an existing attribute.
        inline MetaData& at(const std::wstring& name) { return m_attributes.at(name); }

        //! Access an attribute.
        inline MetaData& operator[](const std::wstring& name) { return m_attributes[name]; }

        //! Whether an attribute exists or not.
        inline bool exists(const std::wstring& name) const { return m_exists && (m_attributes.find(name) != std::end(m_attributes)); }

        //! Whether the data exists (false if cleared or just created).
        inline bool exists() const { return m_exists; }

        //! The type of data.
        inline const std::wstring& type() const { return m_type; }

        //! @}

    private:

        bool m_exists = false;  //!< Whether the data exists or not.
        std::wstring m_type;    //!< The type of data.
        std::unordered_map<std::wstring, MetaData> m_attributes; //!< The attributes.
    };

    //------------------------------//
    //----- External functions -----//

    //! Returns true if we found an existing ElementData of specified type.
    template<class Container>
    inline bool hasOfType(const Container& container, const std::wstring& type)
    {
        for (const auto& data : container)
            if (data.exists() && data.type() == type)
                return true;
        return false;
    }

    //! Returns if found an existing ElementData iterator of specified type, or std::end(container) else.
    template<class Container>
    inline auto findOfType(Container& container, const std::wstring& type) -> decltype(std::end(container))
    {
        return std::find_if(container, [&](const ElementData& data) { return data.exists() && data.type() == type; });
    }
}

