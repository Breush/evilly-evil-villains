#pragma once

#include "tools/time.hpp"

#include <vector>

namespace world
{
    class WorldsData {
    public:
        // Defines world attributes
        struct World {
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
        // TODO void save(const std::string& file);

        // Getters
        std::vector<World>& worlds() { return m_worlds; }

    private:
        std::vector<World> m_worlds;
    };
}
