#pragma once

#include "tools/string.hpp"

#include <libintl.h>
#include <cstdlib>

namespace internationalization
{
    // To be called once at the beginning of the program
    void init(const std::string& language);
}

#define _(STRING) toWString(gettext(STRING))
