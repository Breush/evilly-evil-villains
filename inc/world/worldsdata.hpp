#pragma once

#include "tools/time.hpp"
#include "tools/int.hpp"

#include <vector>

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
            pugi::xml_node* ref;
            std::wstring name;
            std::wstring villain;
            uint dungeons;
            std::wstring mainDungeon;
            time_t created;
            time_t lastPlayed;
            std::string folder;
        };

    public:
        WorldsData();
        virtual ~WorldsData() {}

        // Import/Export
        void load(const std::string& file);
        void save(const std::string& file);

        // Updates
        void updateLastPlayed(World& worldInfo);

        // Getters
        std::vector<World>& worlds() { return m_worlds; }

    private:
        std::vector<World> m_worlds;
    };
}
