#pragma once

#include "tools/metadata.hpp"
#include "tools/tools.hpp"
#include "tools/debug.hpp"

#include <pugixml/pugixml.hpp>
#include <unordered_map>
#include <string>

namespace dungeon
{
    //! A dynamic structure which can be exported and imported as XML.

    class TrapData
    {
    public:

        inline bool exists() const { return m_exists; }
        inline const std::wstring& type() const { return m_type; }

        void clear();
        void saveXML(pugi::xml_node& node);
        void loadXML(const pugi::xml_node& node);

        //! Initialize a empty TrapData of given type.
        void create(std::wstring type);

        //! Returns true if the trap has no attributes.
        inline bool empty() const { return m_attributes.empty(); }

        //! Access an attribute.
        inline MetaData& operator[](const std::wstring& name) { return m_attributes[name]; }

    private:

        bool m_exists = false;
        std::wstring m_type;
        std::unordered_map<std::wstring, MetaData> m_attributes;
    };
}

