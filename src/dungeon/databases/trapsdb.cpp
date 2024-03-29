#include "dungeon/databases/trapsdb.hpp"

#include "core/gettext.hpp"
#include "tools/filesystem.hpp"

#include <pugixml/pugixml.hpp>
#include <stdexcept>

using namespace dungeon;

TrapsDB::TrapsDB()
{
    reload();
}

//------------------------//
//----- Data control -----//

void TrapsDB::reload()
{
    m_trapsData.clear();

    // Loading from XML files
    for (const auto& fileInfo : listFiles("res/vanilla/traps", true)) {
        // Check the file extension
        if (fileInfo.isDirectory || fileExtension(fileInfo.name) != "xml")
            continue;

        // Add its content to the map
        add(fileInfo.fullName);
    }
}

void TrapsDB::add(const std::string& filename)
{
    // Parsing XML
    pugi::xml_document doc;
    doc.load_file(filename.c_str());

    // Check that its a correct file
    const auto& trapNode = doc.child(L"trap");
    if (!trapNode)
        mquit("File '" + filename + "' is not a correct trap data file.");

    // Getting the ID
    std::wstring id = trapNode.attribute(L"id").as_string();
    if (id.empty())
        mquit("Trap ID is unspecified. Cannot add '" + filename + "' file.");
    if (m_trapsData.find(id) != std::end(m_trapsData))
        mquit("Trap ID already exists. Cannot add '" + filename + "' file.");

    // Create the corresponding data
    auto& trapData = m_trapsData[id];
    std::wstring trName = trapNode.attribute(L"trName").as_string();
    trapData.name = _(toString(trName).c_str());
    readRoomFlagsAttribute(trapData.lock, trapNode.attribute(L"lock"));

    // Adding data
    for (const auto& dataNode : trapNode) {
        std::wstring name = dataNode.attribute(L"name").as_string();

        if (name == L"unlockCost")      readCostNode(trapData.unlockCost, dataNode);
        else if (name == L"baseCost")   readCostNode(trapData.baseCost, dataNode);
        else if (name == L"resistance") readResistanceNode(trapData.resistance, dataNode);
        else if (name == L"repairCost") readRelCostNode(trapData.repairCost, dataNode);
    }
}

void TrapsDB::readRoomFlagsAttribute(uint8& lock, const pugi::xml_attribute& attribute)
{
    std::wstring lockString = attribute.as_string();

    if (lockString == L"wall")          lock = RoomFlag::WALL;
    else if (lockString == L"floor")    lock = RoomFlag::FLOOR;
    else if (lockString == L"full")     lock = RoomFlag::WALL | RoomFlag::FLOOR;
    else                                lock = RoomFlag::NONE;
}

void TrapsDB::readCostNode(Cost& cost, const pugi::xml_node& node)
{
    cost.dosh = node.attribute(L"dosh").as_uint();
    cost.soul = node.attribute(L"soul").as_uint();
    cost.fame = node.attribute(L"fame").as_uint();
}

void TrapsDB::readRelCostNode(RelCost& cost, const pugi::xml_node& node)
{
    cost.dosh = node.attribute(L"dosh").as_float();
    cost.soul = node.attribute(L"soul").as_float();
    cost.fame = node.attribute(L"fame").as_float();
}

void TrapsDB::readResistanceNode(TrapResistance& resistance, const pugi::xml_node& node)
{
    resistance.durability = node.attribute(L"durability").as_uint();
}
