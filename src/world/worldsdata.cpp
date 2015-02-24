#include "world/worldsdata.hpp"

#include "tools/debug.hpp"
#include "tools/time.hpp"

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

    // Worlds
    for (auto& worldInfo : root.children(L"world")) {
        World world;

        world.ref = &worldInfo;
        world.name = worldInfo.attribute(L"name").as_string();
        world.villain = worldInfo.attribute(L"villain").as_string();
        world.dungeons = worldInfo.attribute(L"dungeons").as_uint();
        world.mainDungeon = worldInfo.attribute(L"mainDungeon").as_string();
        world.created = time_t(worldInfo.attribute(L"created").as_uint());
        world.lastPlayed = time_t(worldInfo.attribute(L"lastPlayed").as_uint());
        world.folder = internationalization::wstring2string(worldInfo.attribute(L"folder").as_string());

        m_worlds.emplace_back(world);
    }
}

void WorldsData::save(const std::string& file)
{
    // Creating XML
    pugi::xml_document doc;
    auto root = doc.append_child(L"worlds");

    // Worlds
    for (const auto& world : m_worlds) {
        auto worldsInfo = root.append_child(L"world");

        worldsInfo.append_attribute(L"name") = world.name.c_str();
        worldsInfo.append_attribute(L"villain") = world.villain.c_str();
        worldsInfo.append_attribute(L"dungeons") = world.dungeons;
        worldsInfo.append_attribute(L"mainDungeon") = world.mainDungeon.c_str();
        worldsInfo.append_attribute(L"created") = uint(world.created);
        worldsInfo.append_attribute(L"lastPlayed") = uint(world.lastPlayed);
        worldsInfo.append_attribute(L"folder") = internationalization::string2wstring(world.folder).c_str();
    }

    doc.save_file(file.c_str());
    mdebug_application_1("Saving worlds info to " << file);
}

//-------------------//
//----- Updates -----//

void WorldsData::updateLastPlayed(World& worldInfo)
{
    worldInfo.lastPlayed = time(nullptr);
    save("worlds/worlds_saved.xml"); // TODO Remove suffix (used to not compromise svn archive)
}
