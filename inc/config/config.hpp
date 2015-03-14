#pragma once

#include "config/display.hpp"
#include "tools/platform-fixes.hpp" // make_unique

namespace config
{
    inline std::unique_ptr<Display> display() { return std::make_unique<Display>(); }
}
