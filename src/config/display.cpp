#include "config/display.hpp"

#include "tools/tools.hpp"
#include "tools/debug.hpp"

#include <pugixml/pugixml.hpp>
#include <iostream>

using namespace config;

Display::Display()
    : window({false, {1360.f, 768.f}})
    , nui({2u, 3.f})
{
    pugi::xml_document doc;

    #if DEBUG_GLOBAL > 0
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
    // Note: Groups are transparents, we don't really care
    // if a parameter is not in the correct group
    for (auto& group : config.children(L"group")) {
    for (auto& param : group.children(L"param")) {
        std::wstring name = param.attribute(L"name").as_string();

        // Window
        if (name == L"fullscreen") {
            window.fullscreen = param.attribute(L"enabled").as_bool();
        }
        else if (name == L"resolution") {
            window.resolution.x = param.attribute(L"width").as_float();
            window.resolution.y = param.attribute(L"height").as_float();
        }
        else if (name == L"antialiasing") {
            window.antialiasingLevel = param.attribute(L"level").as_uint();
        }

        // NUI
        else if (name == L"size") {
            nui.size = param.attribute(L"value").as_uint();
        }
        else if (name == L"fontFactor") {
            nui.fontFactor = param.attribute(L"value").as_uint();
        }
    }
    }
}

void Display::save()
{
    pugi::xml_node param, group;

    // Creating XML
    pugi::xml_document doc;
    auto config = doc.append_child(L"config");
    config.append_attribute(L"type") = L"display";

    // Window
    group = config.append_child(L"group");
    param.append_attribute(L"name") = L"window";

    param = group.append_child(L"param");
    param.append_attribute(L"name") = L"fullscreen";
    param.append_attribute(L"enabled") = window.fullscreen;

    param = group.append_child(L"param");
    param.append_attribute(L"name") = L"resolution";
    param.append_attribute(L"width") = window.resolution.x;
    param.append_attribute(L"height") = window.resolution.y;

    param = group.append_child(L"param");
    param.append_attribute(L"name") = L"antialiasing";
    param.append_attribute(L"level") = window.antialiasingLevel;

    // NUI
    group = config.append_child(L"group");
    param.append_attribute(L"name") = L"nui";

    param = group.append_child(L"param");
    param.append_attribute(L"name") = L"size";
    param.append_attribute(L"value") = nui.size;

    param = group.append_child(L"param");
    param.append_attribute(L"name") = L"fontFactor";
    param.append_attribute(L"value") = nui.fontFactor;

    #if DEBUG_GLOBAL > 0
        doc.save_file("config/display_saved.xml");
    #else
        doc.save_file("config/display.xml");
    #endif
}

