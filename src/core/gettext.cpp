#include "core/gettext.hpp"

#include <locale>

void internationalization::init()
{
    // General initialisation
    setlocale(LC_ALL, "");

    // Really needed for pugixml... otherwise parsing issues with floating points
    setlocale(LC_NUMERIC, "C");

    // Set text domain
    bindtextdomain("eev", "res/po/");
    textdomain("eev");
}
