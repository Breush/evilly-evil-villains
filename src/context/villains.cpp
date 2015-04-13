#include "context/villains.hpp"

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

//---------------------------//
//----- File management -----//

void Villains::load()
{
    std::wstring file(L"saves/villains.xml");
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

        m_villains.emplace_back(std::move(villain));
    }
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
    }

    doc.save_file(file.c_str());
    wdebug_application_1(L"Saving villains info to " << file);
}

//----------------------//
//----- Management -----//

uint Villains::add(std::wstring name)
{
    Villain villain;

    villain.index = m_villains.size();
    villain.name = std::move(name);

    m_villains.emplace_back(std::move(villain));
    return m_villains.size() - 1u;
}
