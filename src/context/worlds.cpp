#include "context/worlds.hpp"

#include "context/debug.hpp"
#include "tools/string.hpp"
#include "tools/time.hpp"
#include "tools/filesystem.hpp"

#include <pugixml/pugixml.hpp>
#include <stdexcept>
#include <sstream>

using namespace context;

//----------------------------//
//----- Static variables -----//

Worlds context::worlds;

//---------------------------//
//----- File management -----//

void Worlds::load()
{
    #if DEBUG_GLOBAL > 0
        std::wstring file(L"saves/worlds_saved.xml");
        if (!fileExists(file)) file = L"saves/worlds.xml";
    #else
        std::wstring file(L"saves/worlds.xml");
    #endif

    wdebug_context_2(L"Loading worlds info from " << file);

    m_worlds.clear();

    // Parsing XML
    pugi::xml_document doc;
    doc.load_file(file.c_str());

    const auto& root = doc.child(L"worlds");
    if (!root) throw std::runtime_error("File " + toString(file) + " is not a valid worlds file.");

    // Worlds
    for (auto& worldInfo : root.children(L"world")) {
        World world;

        world.index = m_worlds.size();
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

void Worlds::save()
{
    #if DEBUG_GLOBAL > 0
        std::wstring file(L"saves/worlds_saved.xml");
    #else
        std::wstring file(L"saves/worlds.xml");
    #endif

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
    wdebug_context_1(L"Saving worlds info to " << file);
}

//-------------------------------//
//----- Setters and getters -----//

const Worlds::World& Worlds::select(uint index)
{
    massert(index < m_worlds.size(), "Index " << index << " is too big.");
    m_selected = &get(index);
    wdebug_context_1(L"Selected world #" << index << L" named " << m_selected->name);
    return *m_selected;
}

//----------------------//
//----- Management -----//

uint Worlds::add(std::wstring name, std::wstring villain)
{
    wdebug_context_1(L"Creating world " << name << L" with villain " << villain);

    World world;

    world.index = m_worlds.size();
    world.name = std::move(name);
    world.villain = std::move(villain);
    world.dungeons = 0u;
    world.mainDungeon = L"Unknown";
    world.created = time(nullptr);
    world.lastPlayed = world.created;

    // Creating folder
    world.folder = world.name;
    filterSpecial(world.folder);

    // Automatically increase folder postfix if we cannot create directory.
    std::wstring folderPostfix(L"/");
    uint iFolderPostfix = 0;
    while (!createDirectory(L"saves/" + world.folder + folderPostfix)) {
        std::wstringstream str;
        str << L"_" << ++iFolderPostfix;
        folderPostfix = str.str() + L"/";

        // If it seems like we cannot create directories, just throw...
        if (iFolderPostfix > 42)
            throw std::runtime_error("Cannot create directories inside saves/.");
    }

    world.folder += folderPostfix;

    m_worlds.emplace_back(std::move(world));
    return m_worlds.size() - 1u;
}

void Worlds::remove(const std::wstring& villain)
{
    for (auto it = m_worlds.begin(); it != m_worlds.end();) {
        if (it->villain == villain) it = m_worlds.erase(it);
        else ++it;
    }
}

uint Worlds::count(const std::wstring& villain)
{
    uint worldsCount = 0u;

    for (auto& world : m_worlds)
        if (world.villain == villain)
            ++worldsCount;

    return worldsCount;
}

void Worlds::refreshLastPlayed()
{
    m_worlds[m_selected->index].lastPlayed = time(nullptr);
}

