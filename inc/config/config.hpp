#pragma once

#include "tools/platform-fixes.hpp" // make_unique

namespace config
{
    class Display;

    inline std::unique_ptr<Display> display() { return std::make_unique<Display>(); }
}
