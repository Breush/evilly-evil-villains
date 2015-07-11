#include "context/villains.hpp"

#include "context/worlds.hpp"
#include "tools/string.hpp"
#include "tools/debug.hpp"
#include "tools/time.hpp"
#include "tools/filesystem.hpp"

#include <pugixml/pugixml.hpp>
#include <stdexcept>
#include <sstream>

using namespace context;

//----------------------------//
//----- Static variables -----//

Villains context::villains;

//-------------------//
//----- Villain -----//

uint Villain::worldsCount() const
{
    return worlds.count(name);
}

//---------------------------//
//----- File management -----//

void Villains::load()
{
    #if DEBUG_GLOBAL > 0
        std::wstring file(L"saves/villains_saved.xml");
        if (!fileExists(file)) file = L"saves/villains.xml";
        std::wcout << L"|DEBUG| Loading villains file '" << file << L"'." << std::endl;
    #else
        std::wstring file(L"saves/villains.xml");
    #endif

    m_villains.clear();

    // Parsing XML
    pugi::xml_document doc;
    doc.load_file(file.c_str());

    const auto& root = doc.child(L"villains");
    if (!root) throw std::runtime_error("File " + toString(file) + " is not a valid villain file.");

    // Worlds
    for (auto& info : root.children(L"villain")) {
        Villain villain;

        villain.index = m_villains.size();
        villain.name = info.attribute(L"name").as_string();
        villain.doshWallet.set(info.attribute(L"dosh").as_uint());

        m_villains.emplace_back(std::move(villain));
    }

    worlds.load();
}

void Villains::save()
{
    #if DEBUG_GLOBAL > 0
        std::wstring file(L"saves/villains_saved.xml");
    #else
        std::wstring file(L"saves/villains.xml");
    #endif

    // Creating XML
    pugi::xml_document doc;
    auto root = doc.append_child(L"villains");

    // Worlds
    for (const auto& villain : m_villains) {
        auto info = root.append_child(L"villain");

        info.append_attribute(L"name") = villain.name.c_str();
        info.append_attribute(L"dosh") = villain.doshWallet.value();
    }

    doc.save_file(file.c_str());
    wdebug_application_1(L"Saving villains info to " << file);

    worlds.save();
}

//----------------------//
//----- Management -----//

uint Villains::add(std::wstring name)
{
    wdebug_application_1(L"Created villain " << name << L".");

    Villain villain;

    villain.index = m_villains.size();
    villain.name = std::move(name);

    m_villains.emplace_back(std::move(villain));
    return m_villains.size() - 1u;
}

void Villains::remove(uint index)
{
    wdebug_application_1(L"Deleted villain " << m_villains[index].name << L" and all its worlds.");

    removeWorlds(index);
    m_villains.erase(m_villains.begin() + index);

    for (auto i = index; i < m_villains.size(); ++i)
        m_villains[i].index = i;
}

void Villains::removeWorlds(uint index)
{
    worlds.remove(m_villains[index].name);
}

Villain* Villains::getFromVillainName(const std::wstring& villainName)
{
    for (auto& villain : m_villains)
        if (villain.name == villainName)
            return &villain;

    return nullptr;
}

Villain* Villains::getFromWorldFolder(const std::wstring& folder)
{
    for (auto& world : worlds.get())
        if (world.folder == folder)
            return getFromVillainName(world.villain);

    return nullptr;
}
