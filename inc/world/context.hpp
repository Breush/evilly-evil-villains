#pragma once

#include "world/worldsdata.hpp"

namespace world
{
    //----- Context -----//

    class Context {
    public:
        Context() {}
        ~Context() {}

        // Interaction
        void updateLastPlayed();

        // Attributes
        WorldsData::World* info; //< The current selected world
        WorldsData worldsData;  //< The global worlds infos
    };

    extern Context context;
}
