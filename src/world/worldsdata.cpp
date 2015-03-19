#include "world/worldsdata.hpp"

#include "tools/string.hpp"
#include "tools/debug.hpp"
#include "tools/time.hpp"
#include "tools/filesystem.hpp"

#include <pugixml.hpp>
#include <stdexcept>

using namespace world;

WorldsData::WorldsData()
{
}

void WorldsData::load(const std::wstring& file)
{
    m_worlds.clear();

    // Parsing XML
    pugi::xml_document doc;
    doc.load_file(file.c_str());

    const auto& root = doc.child(L"worlds");
    if (!root) throw std::runtime_error("File " + toString(file) + " is not a valid worlds file.");

    // Worlds
    for (auto& worldInfo : root.children(L"world")) {
        World world;

        //world.ref = &worldInfo;
        world.name = worldInfo.attribute(L"name").as_string();
        world.villain = worldInfo.attribute(L"villain").as_string();
        world.dungeons = worldInfo.attribute(L"dungeons").as_uint();
        world.mainDungeon = worldInfo.attribute(L"mainDungeon").as_string();
        world.created = static_cast<time_t>(worldInfo.attribute(L"created").as_uint());
        world.lastPlayed = static_cast<time_t>(worldInfo.attribute(L"lastPlayed").as_uint());
        world.folder = worldInfo.attribute(L"folder").as_string();

        m_worlds.emplace_back(std::move(world));
    }
}

void WorldsData::save(const std::wstring& file)
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
        worldsInfo.append_attribute(L"folder") = world.folder.c_str();
    }

    doc.save_file(file.c_str());
    wdebug_application_1(L"Saving worlds info to " << file);
}

uint WorldsData::createWorld(const std::wstring& worldName)
{
    World world;

    // TODO Complete information
    world.name = worldName;
    world.villain = L"Unknown";
    world.dungeons = 0u;
    world.mainDungeon = L"Unknown";
    world.created = time(nullptr);
    world.lastPlayed = world.created;

    // Creating folder
    world.folder = world.name;
    filterSpecial(world.folder);
    world.folder += L"/";

    if (!createDirectory(L"worlds/" + world.folder)) {
        // TODO Manage user feedback (or automatically rename folder)
        throw std::runtime_error("Cannot create directory worlds/" + toString(world.folder));
    }

    m_worlds.emplace_back(std::move(world));
    return m_worlds.size() - 1u;
}

//-------------------//
//----- Updates -----//

void WorldsData::updateLastPlayed(World& worldInfo)
{
    worldInfo.lastPlayed = time(nullptr);

    #if DEBUG_GLOBAL > 0
        save(L"worlds/worlds_saved.xml");
    #else
        save();
    #endif
}
