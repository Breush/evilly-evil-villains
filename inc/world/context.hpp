#pragma once

#include "world/worldsdata.hpp"

namespace world
{
    //----- Context -----//

    struct Context {
        Context() {}
        ~Context() {}

        WorldsData::World info;
    };

    extern Context context;
}
