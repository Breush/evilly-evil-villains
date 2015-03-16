#include "config/display.hpp"

#include "tools/tools.hpp"
#include "tools/debug.hpp"

#include <pugixml.hpp>

using namespace config;

Display::Display()
    : fullscreen(false)
    , resolution(1024.f, 576.f) // 16/9
    , nui(2)
{
    pugi::xml_document doc;

    #if DEBUG_GLOBAL
        doc.load_file("config/display_debug.xml");
    #else
        doc.load_file("config/display.xml");
    #endif

    // Checks if we read the file OK
    const auto& config = doc.child(L"config");
    if (!config || config.attribute(L"type").as_string() != std::wstring(L"display")) {
        std::cerr << "[CONFIG] Could not find valid display config file. Using default parameters." << std::endl;
        return;
    }

    // File is OK, parsing it
    for (auto& param : config.children(L"param")) {
        std::wstring name = param.attribute(L"name").as_string();

        // Fullscreen
        if (name == L"fullscreen") {
            fullscreen = param.attribute(L"enabled").as_bool();
        }

        // Resolution
        else if (name == L"resolution") {
            resolution.x = param.attribute(L"width").as_float();
            resolution.y = param.attribute(L"height").as_float();
        }

        // NUI
        else if (name == L"nui") {
            nui = param.attribute(L"size").as_uint();
        }
    }
}

void Display::save()
{
    pugi::xml_node param;

    // Creating XML
    pugi::xml_document doc;
    auto config = doc.append_child(L"config");
    config.append_attribute(L"type") = L"display";

    // Fullscreen
    param = config.append_child(L"param");
    param.append_attribute(L"name") = L"fullscreen";
    param.append_attribute(L"enabled") = fullscreen;

    // Resolution
    param = config.append_child(L"param");
    param.append_attribute(L"name") = L"resolution";
    param.append_attribute(L"width") = resolution.x;
    param.append_attribute(L"height") = resolution.y;

    // NUI
    param = config.append_child(L"param");
    param.append_attribute(L"name") = L"nui";
    param.append_attribute(L"size") = nui;

    #if DEBUG_GLOBAL
        doc.save_file("config/display_saved.xml");
    #else
        doc.save_file("config/display.xml");
    #endif
}

