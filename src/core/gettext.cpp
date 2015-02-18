#include "core/gettext.hpp"

#include <locale>

void internationalization::init(const std::string& language)
{
	// Force a specific language
	if (!language.empty()) {
		setenv("LANG", language.c_str(), 1);
		setenv("LANGUAGE", language.c_str(), 1);
	}
	
    // General initialisation
    setlocale(LC_ALL, "");

    // Really needed for pugixml... otherwise parsing issues with floating points
    setlocale(LC_NUMERIC, "C");

    // Set text domain
    bindtextdomain("eev", "res/po/");
    textdomain("eev");
}
