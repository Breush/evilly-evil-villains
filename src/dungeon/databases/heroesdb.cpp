#include "dungeon/databases/heroesdb.hpp"

#include "core/gettext.hpp"
#include "tools/filesystem.hpp"

#include <pugixml/pugixml.hpp>
#include <stdexcept>

using namespace dungeon;

HeroesDB::HeroesDB()
{
    reload();
}

//------------------------//
//----- Data control -----//

void HeroesDB::reload()
{
    m_heroesData.clear();

    // Loading from XML files
    for (const auto& fileInfo : listFiles("res/vanilla/heroes", true)) {
        // Check the file extension
        if (fileInfo.isDirectory || fileExtension(fileInfo.name) != "xml")
            continue;

        // Add its content to the map
        add(fileInfo.fullName);
    }
}

void HeroesDB::add(const std::string& filename)
{
    // Parsing XML
    pugi::xml_document doc;
    doc.load_file(filename.c_str());

    // Check that its a correct file
    const auto& heroNode = doc.child(L"hero");
    if (!heroNode)
        throw std::logic_error("File '" + filename + "' is not a correct hero data file.");

    // Getting the ID
    std::wstring id = heroNode.attribute(L"id").as_string();
    if (id.empty())
        throw std::logic_error("Hero ID is unspecified. Cannot add '" + filename + "' file.");
    if (m_heroesData.find(id) != std::end(m_heroesData))
        throw std::logic_error("Hero ID already exists. Cannot add '" + filename + "' file.");

    // Create the corresponding data
    auto& heroData = m_heroesData[id];
    std::wstring trName = heroNode.attribute(L"trName").as_string();
    heroData.name = _(toString(trName).c_str());

    // Adding data
    for (const auto& dataNode : heroNode) {
        std::wstring name = dataNode.attribute(L"name").as_string();

        if (name == L"startingHP")      heroData.startingHP = dataNode.attribute(L"value").as_float();
        else if (name == L"speed")      readSpeedNode(heroData.speed, dataNode);
        else if (name == L"pause")      heroData.pauseDelay = dataNode.attribute(L"delay").as_float();
    }
}

void HeroesDB::readSpeedNode(sf::Vector2f& speed, const pugi::xml_node& node)
{
    speed.x = node.attribute(L"vx").as_float();
    speed.y = node.attribute(L"vy").as_float();
}
