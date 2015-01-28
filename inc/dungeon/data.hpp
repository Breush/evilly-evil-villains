#pragma once

#include <string>

namespace dungeon
{
    class Data
    {
    public:
        Data();
        virtual ~Data() {}

        // Import
        void load(const std::string& file);
    };
}
