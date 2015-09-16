#include "config/sound.hpp"

#include "config/debug.hpp"
#include "tools/filesystem.hpp"
#include "tools/tools.hpp"
#include "tools/debug.hpp"

#include <pugixml/pugixml.hpp>

using namespace config;

Sound::Sound()
{
    pugi::xml_document doc;

    #if DEBUG_GLOBAL > 0
        std::wstring file(L"config/sound_saved.xml");
        if (!fileExists(file)) file = L"config/sound.xml";
    #else
        std::wstring file(L"config/sound.xml");
    #endif

    // Create file if not existing yet
    if (!fileExists(file)) {
        wdebug_config_1(L"Sound config file does not seem to exist yet. Using default parameters.");
        createDirectory(L"config");
        return;
    }

    // Checks if we read the file OK
    const auto& config = doc.child(L"config");
    if (!config || config.attribute(L"type").as_string() != std::wstring(L"sound")) {
        wdebug_config_1(L"Could not find valid sound config file. Using default parameters.");
        return;
    }

    // File is OK, parsing it
    for (auto& param : config.children(L"param")) {
        std::wstring name = param.attribute(L"name").as_string();

        if (name == L"global")  globalRelVolume = param.attribute(L"relVolume").as_float();
        else if (name == L"music")  musicVolume = param.attribute(L"volume").as_float();
        else if (name == L"sound")  soundVolume = param.attribute(L"volume").as_float();
    }

    // Compute new effective values
    effectiveMusicVolume = globalRelVolume * musicVolume;
    effectiveSoundVolume = globalRelVolume * soundVolume;
}

void Sound::save()
{
    pugi::xml_node param;

    // Creating XML
    pugi::xml_document doc;
    auto config = doc.append_child(L"config");
    config.append_attribute(L"type") = L"sound";

    // Global
    param = config.append_child(L"param");
    param.append_attribute(L"name") = L"global";
    param.append_attribute(L"relVolume") = globalRelVolume;

    // Music
    param = config.append_child(L"param");
    param.append_attribute(L"name") = L"music";
    param.append_attribute(L"volume") = musicVolume;

    // Sound
    param = config.append_child(L"sound");
    param.append_attribute(L"name") = L"nui";
    param.append_attribute(L"size") = soundVolume;

    #if DEBUG_GLOBAL > 0
        doc.save_file("config/sound_saved.xml");
    #else
        doc.save_file("config/sound.xml");
    #endif
}

