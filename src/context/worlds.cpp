#include "context/worlds.hpp"

#include "context/debug.hpp"
#include "tools/string.hpp"
#include "tools/time.hpp"
#include "tools/filesystem.hpp"
#include "tools/platform-fixes.hpp" // erase_if

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

    // Create file if not existing yet
    if (!fileExists(file)) {
        wdebug_context_1(L"Worlds file does not seem to exist yet. Using default one.");
        createDirectory(L"saves");
        return;
    }

    // Parsing XML
    pugi::xml_document doc;
    doc.load_file(file.c_str());

    const auto& root = doc.child(L"worlds");
    if (!root) mquit("File " + toString(file) + " is not a valid worlds file.");

    // Worlds
    for (auto& worldInfo : root.children(L"world")) {
        World world;

        world.index = m_worlds.size();
        world.name = worldInfo.attribute(L"name").as_string();
        world.villain = worldInfo.attribute(L"villain").as_string();
        world.gamemode = static_cast<Gamemode>(worldInfo.attribute(L"gamemode").as_uint());
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
        worldsInfo.append_attribute(L"gamemode") = static_cast<uint>(world.gamemode);
        worldsInfo.append_attribute(L"created") = static_cast<uint>(world.created);
        worldsInfo.append_attribute(L"lastPlayed") = static_cast<uint>(world.lastPlayed);
        worldsInfo.append_attribute(L"folder") = world.folder.c_str();
    }

    doc.save_file(file.c_str());
    wdebug_context_1(L"Saving worlds info to " << file);
}

//-------------------------------//
//----- Setters and getters -----//

Worlds::World& Worlds::select(uint index)
{
    massert(index < m_worlds.size(), "Index " << index << " is too big.");
    m_selected = &m_worlds[index];
    wdebug_context_1(L"Selected world #" << index << L" named " << m_selected->name);
    return *m_selected;
}

//----------------------//
//----- Management -----//

uint Worlds::add()
{
    wdebug_context_1(L"Creating new world.");

    World world;
    world.index = m_worlds.size();
    world.created = time(nullptr);
    world.lastPlayed = world.created;

    m_worlds.emplace_back(std::move(world));
    return m_worlds.size() - 1u;
}

void Worlds::setNameCreation(World& world, std::wstring name)
{
    world.name = std::move(name);

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

        // If it seems like we cannot create directories, just throw an error then...
        if (iFolderPostfix > 42)
            mquit("Cannot create directories inside saves/.");
    }

    world.folder += folderPostfix;
}

void Worlds::remove(const World& world)
{
    std::erase_if(m_worlds, [&world] (const World& inWorld) { return &inWorld == &world; });
}

void Worlds::removeFromFolder(const std::wstring& folder)
{
    std::erase_if(m_worlds, [&folder] (const World& world) { return world.folder == folder; });
}

void Worlds::removeFromVillain(const std::wstring& villain)
{
    std::erase_if(m_worlds, [&villain] (const World& world) { return world.villain == villain; });
}

uint Worlds::countFromVillain(const std::wstring& villain)
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

