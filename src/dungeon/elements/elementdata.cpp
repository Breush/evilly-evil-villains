#include "dungeon/elements/elementdata.hpp"

#include <stdexcept>

using namespace dungeon;

//------------------------//
//----- Manipulation -----//

void ElementData::clear()
{
    m_exists = false;
    m_attributes.clear();
}

void ElementData::create(std::wstring type)
{
    m_attributes.clear();
    m_type = std::move(type);
    m_exists = true;
}

bool ElementData::exists(const std::wstring& name) const
{
    returnif (!m_exists) false;
    auto found = m_attributes.find(name);
    returnif (found == std::end(m_attributes)) false;
    return !found->second.type().empty();
}

//---------------------------//
//----- XML interaction -----//

void ElementData::saveXML(pugi::xml_node& node) const
{
    returnif (!m_exists);

    // Create the trap child.
    node.append_attribute(L"type") = m_type.c_str();

    // Export all its attributes
    for (const auto& attribute : m_attributes) {
        const auto& type = attribute.second.type();
        auto child = node.append_child(attribute.first.c_str());
        child.append_attribute(L"type") = type.c_str();

        if (type == L"bool")       child.append_attribute(L"value") = attribute.second.as_bool();

        else if (type == L"int8")  child.append_attribute(L"value") = static_cast<int>(attribute.second.as_int8());
        else if (type == L"int16") child.append_attribute(L"value") = static_cast<int>(attribute.second.as_int16());
        else if (type == L"int32") child.append_attribute(L"value") = static_cast<int>(attribute.second.as_int32());
        else if (type == L"int64") child.append_attribute(L"value") = static_cast<long long>(attribute.second.as_int64());

        else if (type == L"uint8")  child.append_attribute(L"value") = static_cast<unsigned int>(attribute.second.as_uint8());
        else if (type == L"uint16") child.append_attribute(L"value") = static_cast<unsigned int>(attribute.second.as_uint16());
        else if (type == L"uint32") child.append_attribute(L"value") = static_cast<unsigned int>(attribute.second.as_uint32());
        else if (type == L"uint64") child.append_attribute(L"value") = static_cast<unsigned long long>(attribute.second.as_uint64());

        else if (type == L"float")  child.append_attribute(L"value") = attribute.second.as_float();
        else if (type == L"double") child.append_attribute(L"value") = attribute.second.as_double();

        else if (type == L"v2uint8") {
            const auto& v = attribute.second.as_v2uint8();
            child.append_attribute(L"x") = static_cast<unsigned int>(v.x);
            child.append_attribute(L"y") = static_cast<unsigned int>(v.y);
        }
        else if (type == L"v2float") {
            const auto& v = attribute.second.as_v2float();
            child.append_attribute(L"x") = v.x;
            child.append_attribute(L"y") = v.y;
        }

        else mquit("Some MetaData (saveXML) has an invalid type.");
    }
}

void ElementData::loadXML(const pugi::xml_node& node)
{
    // Reset state
    clear();

    // Read trap type name
    m_exists = true;
    m_type = node.attribute(L"type").as_string();

    // And all its attributes
    for (const auto& child : node.children()) {
        // Is is a valid elementData attribute?
        if (!child.attribute(L"type"))
            continue;

        std::wstring name = child.name();
        std::wstring type = child.attribute(L"type").as_string();
        m_attributes[name].setType(type);

        if (type == L"bool")       m_attributes[name] = child.attribute(L"value").as_bool();

        else if (type == L"int8")  m_attributes[name] = static_cast<int8>(child.attribute(L"value").as_int());
        else if (type == L"int16") m_attributes[name] = static_cast<int16>(child.attribute(L"value").as_int());
        else if (type == L"int32") m_attributes[name] = static_cast<int32>(child.attribute(L"value").as_int());
        else if (type == L"int64") m_attributes[name] = static_cast<int64>(child.attribute(L"value").as_llong());

        else if (type == L"uint8")  m_attributes[name] = static_cast<uint8>(child.attribute(L"value").as_uint());
        else if (type == L"uint16") m_attributes[name] = static_cast<uint16>(child.attribute(L"value").as_uint());
        else if (type == L"uint32") m_attributes[name] = static_cast<uint32>(child.attribute(L"value").as_uint());
        else if (type == L"uint64") m_attributes[name] = static_cast<uint64>(child.attribute(L"value").as_ullong());

        else if (type == L"float")  m_attributes[name] = child.attribute(L"value").as_float();
        else if (type == L"double") m_attributes[name] = child.attribute(L"value").as_double();

        else if (type == L"v2uint8") {
            Vec2<uint8> v;
            v.x = child.attribute(L"x").as_uint();
            v.y = child.attribute(L"y").as_uint();
            m_attributes[name] = v;
        }
        else if (type == L"v2float") {
            Vec2<float> v;
            v.x = child.attribute(L"x").as_float();
            v.y = child.attribute(L"y").as_float();
            m_attributes[name] = v;
        }
        else mquit("Some MetaData (loadXML) has an invalid type.");
    }
}
