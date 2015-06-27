#include "dungeon/trapdata.hpp"

#include <stdexcept>

using namespace dungeon;

//------------------------//
//----- Manipulation -----//

void TrapData::clear()
{
    m_exists = false;
    m_attributes.clear();
}

void TrapData::create(std::wstring type)
{
    m_attributes.clear();
    m_type = std::move(type);
    m_exists = true;
}

//---------------------------//
//----- XML interaction -----//

void TrapData::saveXML(pugi::xml_node& node)
{
    returnif (!m_exists);

    // Create the trap child.
    node.append_attribute(L"type") = m_type.c_str();

    std::wcout << L"Save " << m_type << std::endl;

    // Export all its attributes
    for (const auto& attribute : m_attributes) {
        const auto& type = attribute.second.type();
        auto child = node.append_child(attribute.first.c_str());

        child.append_attribute(L"type") = type.c_str();
        auto value = child.append_attribute(L"value");

        if (type == L"int8")       value = static_cast<int>(attribute.second.as_int8());
        else if (type == L"int16") value = static_cast<int>(attribute.second.as_int16());
        else if (type == L"int32") value = static_cast<int>(attribute.second.as_int32());
        else if (type == L"int64") value = static_cast<long long>(attribute.second.as_int64());

        else if (type == L"uint8")  value = static_cast<unsigned int>(attribute.second.as_uint8());
        else if (type == L"uint16") value = static_cast<unsigned int>(attribute.second.as_uint16());
        else if (type == L"uint32") value = static_cast<unsigned int>(attribute.second.as_uint32());
        else if (type == L"uint64") value = static_cast<unsigned long long>(attribute.second.as_uint64());

        else throw std::logic_error("Some MetaData has an invalid type.");
    }
}

void TrapData::loadXML(const pugi::xml_node& node)
{
    // Reset state
    clear();

    // Read trap type name
    m_exists = true;
    m_type = node.attribute(L"type").as_string();

    // And all its attributes
    for (const auto& child : node.children()) {
        std::wstring name = child.name();
        std::wstring type = child.attribute(L"type").as_string();

        m_attributes[name].setType(type);

        if (type == L"int8")       m_attributes[name] = static_cast<int8>(child.attribute(L"value").as_int());
        else if (type == L"int16") m_attributes[name] = static_cast<int16>(child.attribute(L"value").as_int());
        else if (type == L"int32") m_attributes[name] = static_cast<int32>(child.attribute(L"value").as_int());
        else if (type == L"int64") m_attributes[name] = static_cast<int64>(child.attribute(L"value").as_llong());

        else if (type == L"uint8")  m_attributes[name] = static_cast<uint8>(child.attribute(L"value").as_uint());
        else if (type == L"uint16") m_attributes[name] = static_cast<uint16>(child.attribute(L"value").as_uint());
        else if (type == L"uint32") m_attributes[name] = static_cast<uint32>(child.attribute(L"value").as_uint());
        else if (type == L"uint64") m_attributes[name] = static_cast<uint64>(child.attribute(L"value").as_ullong());
    }
}

//----------------//
//----- Cost -----//

uint TrapData::onCreateCost() const
{
    returnif (!m_exists) 0u;

    if (m_type == L"pickpock") return 220u;
    else throw std::logic_error("Some trap has not been registered for onCreateDosh().");

    return 0u;
}

uint TrapData::onDestroyGain() const
{
    returnif (!m_exists) 0u;

    if (m_type == L"pickpock") return 30u + m_attributes.at(L"dosh").as_uint32();
    else throw std::logic_error("Some trap has not been registered for onCreateDosh().");

    return 0u;
}

