#pragma once

#include "tools/int.hpp"

#include <string>
#include <vector>
#include <ctime>

// Forward declarations
namespace pugi
{
    class xml_node;
}

namespace world
{
    class WorldsData {
    public:
        // Defines world attributes
        struct World {
            //pugi::xml_node* ref;
            std::wstring name;
            std::wstring villain;
            uint dungeons;
            std::wstring mainDungeon;
            time_t created;
            time_t lastPlayed;
            std::wstring folder;
        };

    public:
        WorldsData();
        virtual ~WorldsData() = default;

        // Import/Export
        void load(const std::wstring& file = L"worlds/worlds.xml");
        void save(const std::wstring& file = L"worlds/worlds.xml");

        // Updates
        void updateLastPlayed(World& worldInfo);

        // Worlds management
        uint createWorld(const std::wstring& worldName);

        // Getters
        std::vector<World>& worlds() { return m_worlds; }

    private:
        std::vector<World> m_worlds;
    };
}
