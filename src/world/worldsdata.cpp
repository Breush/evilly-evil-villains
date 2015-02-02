#include "world/worldsdata.hpp"

#include <pugixml.hpp>
#include <stdexcept>

using namespace world;

WorldsData::WorldsData()
{
}

void WorldsData::load(const std::string& file)
{
    m_worlds.clear();

    // Parsing XML
    pugi::xml_document doc;
    doc.load_file(file.c_str());

    const auto& root = doc.child(L"worlds");
    if (!root) throw std::runtime_error("File " + file + " is not a valid worlds file.");

    // Dungeon
    for (auto& worldInfo : root.children(L"world")) {
        World world;

        world.name = worldInfo.attribute(L"name").as_string();
        world.villain = worldInfo.attribute(L"villain").as_string();
        world.dungeons = worldInfo.attribute(L"dungeons").as_uint();
        world.mainDungeon = worldInfo.attribute(L"mainDungeon").as_string();
        world.created = worldInfo.attribute(L"created").as_uint();
        world.lastPlayed = worldInfo.attribute(L"lastPlayed").as_uint();
        world.folder = internationalization::wstring2string(worldInfo.attribute(L"folder").as_string());

        m_worlds.push_back(world);
    }
}
