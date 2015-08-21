#pragma once

#include "tools/string.hpp"

#include <libintl.h>
#include <cstdlib>

namespace internationalization
{
    //! To be called when changing language.
    void init(const std::string& language);

    //! To be called once when closing the program.
    void close();
}

#define _(STRING) toWString(std::string(gettext(STRING)))
