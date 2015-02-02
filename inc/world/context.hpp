#pragma once

#include "world/worldsdata.hpp"

#include <iostream> // FIXME

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
