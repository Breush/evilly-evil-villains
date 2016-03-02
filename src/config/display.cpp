#include "config/display.hpp"

#include "config/debug.hpp"
#include "tools/filesystem.hpp"
#include "tools/tools.hpp"

#include <pugixml/pugixml.hpp>
#include <iostream>

using namespace config;

Display::Display()
    : window({true, false, {1360.f, 768.f}, 1})
    , nui({2u, 1.f})
    , global({L"en_EN", 20.f, 0.05f})
{
    pugi::xml_document doc;

    #if DEBUG_GLOBAL > 0
        std::wstring file(L"config/display_saved.xml");
        if (!fileExists(file)) file = L"config/display.xml";
    #else
        std::wstring file(L"config/display.xml");
    #endif

    // Create file if not existing yet
    if (!fileExists(file)) {
        wdebug_config_1(L"Display config file does not seem to exist yet. Using default parameters.");
        createDirectory(L"config");
        return;
    }

    // Checks if we read the file OK
    doc.load_file(file.c_str());
    const auto& config = doc.child(L"config");
    if (!config || config.attribute(L"type").as_string() != std::wstring(L"display")) {
        wdebug_config_1(L"Could not find valid display config file. Using default parameters.");
        return;
    }

    // File is OK, parsing it
    // Note: Groups are transparent, we don't really care
    // if a parameter is not in the correct group
    for (auto& group : config.children(L"group"))
    for (auto& param : group.children(L"param")) {
        std::wstring name = param.attribute(L"name").as_string();

        // Window
        if (name == L"vsync") {
            window.vsync = param.attribute(L"enabled").as_bool();
        }
        else if (name == L"fullscreen") {
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
            nui.fontFactor = param.attribute(L"value").as_float();
        }

        // Global
        else if (name == L"language") {
            global.language = param.attribute(L"code").as_string();
        }
        else if (name == L"scrollingFactor") {
            global.scrollingFactor = param.attribute(L"value").as_float();
        }
        else if (name == L"zoomSpeed") {
            global.zoomSpeed = param.attribute(L"value").as_float();
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
    group.append_attribute(L"type") = L"window";

    param = group.append_child(L"param");
    param.append_attribute(L"name") = L"vsync";
    param.append_attribute(L"enabled") = window.vsync;

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
    group.append_attribute(L"type") = L"nui";

    param = group.append_child(L"param");
    param.append_attribute(L"name") = L"size";
    param.append_attribute(L"value") = nui.size;

    param = group.append_child(L"param");
    param.append_attribute(L"name") = L"fontFactor";
    param.append_attribute(L"value") = nui.fontFactor;

    // Global
    group = config.append_child(L"group");
    group.append_attribute(L"type") = L"global";

    param = group.append_child(L"param");
    param.append_attribute(L"name") = L"language";
    param.append_attribute(L"code") = global.language.c_str();

    param = group.append_child(L"param");
    param.append_attribute(L"name") = L"scrollingFactor";
    param.append_attribute(L"value") = global.scrollingFactor;

    param = group.append_child(L"param");
    param.append_attribute(L"name") = L"zoomSpeed";
    param.append_attribute(L"value") = global.zoomSpeed;

    #if DEBUG_GLOBAL > 0
        doc.save_file("config/display_saved.xml");
    #else
        doc.save_file("config/display.xml");
    #endif
}

