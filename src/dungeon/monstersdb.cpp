#include "dungeon/monstersdb.hpp"

#include "tools/filesystem.hpp"

#include <pugixml/pugixml.hpp>
#include <stdexcept>

using namespace dungeon;

MonstersDB::MonstersDB()
{
    // Loading from XML files
    for (const auto& fileInfo : listFiles("res/data", true)) {
        // Check the file extension
        if (fileInfo.isDirectory || fileExtension(fileInfo.name) != "xml")
            continue;

        // Add its content to the map
        add(fileInfo.fullName);
    }
}

//------------------------//
//----- Data control -----//

void MonstersDB::add(const std::string& filename)
{
    // Parsing XML
    pugi::xml_document doc;
    doc.load_file(filename.c_str());

    // Check that its a correct file
    const auto& monsterNode = doc.child(L"monster");
    if (!monsterNode)
        throw std::logic_error("File '" + filename + "' is not a correct monster data file.");

    // Getting the ID
    std::wstring id = monsterNode.attribute(L"id").as_string();
    if (id.empty())
        throw std::logic_error("Monster ID is unspecified. Cannot add '" + filename + "' file.");
    if (m_monstersData.find(id) != std::end(m_monstersData))
        throw std::logic_error("Monster ID already exists. Cannot add '" + filename + "' file.");

    // Create the corresponding data
    auto& monsterData = m_monstersData[id];

    // Adding data
    for (const auto& dataNode : monsterNode) {
        std::wstring name = dataNode.attribute(L"name").as_string();

        if (name == L"baseCost")            readCostNode(monsterData.baseCost, dataNode);
        else if (name == L"monthlyCost")    readCostNode(monsterData.monthlyCost, dataNode);
        else if (name == L"speed")          readSpeedNode(monsterData.speed, dataNode);
        else if (name == L"pause")          monsterData.pauseDelay = dataNode.attribute(L"delay").as_float();
    }
}

void MonstersDB::readCostNode(Cost& cost, const pugi::xml_node& node)
{
    cost.dosh = node.attribute(L"dosh").as_uint();
    cost.soul = node.attribute(L"soul").as_uint();
    cost.fame = node.attribute(L"fame").as_uint();
}

void MonstersDB::readSpeedNode(sf::Vector2f& speed, const pugi::xml_node& node)
{
    speed.x = node.attribute(L"vx").as_float();
    speed.y = node.attribute(L"vy").as_float();
}
