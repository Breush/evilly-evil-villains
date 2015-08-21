// MinGW does not declare _putenv() because of -std=c++11,
// this fix enables it.
#ifdef __WIN32__
    #undef __STRICT_ANSI__
#endif

#include "core/gettext.hpp"

#include "core/debug.hpp"

#include <cstdlib>
#include <cstring>
#include <locale>

//----------------------------//
//----- Static variables -----//

static char* s_lang = nullptr;
static char* s_language = nullptr;

//---------------------------//
//----- Internalization -----//

void internationalization::init(const std::string& language)
{
    mdebug_core_1("Setting language to " << language);

    // Free memory if any
    close();

    // Force a specific language to the environment
    std::string sLang("LANG=" + language);
    std::string sLanguage("LANGUAGE=" + language);

    s_lang = new char[sLang.size() + 1u];
    s_language = new char[sLanguage.size() + 1u];

    strcpy(s_lang, sLang.c_str());
    strcpy(s_language, sLanguage.c_str());

    #ifdef __WIN32__
    _putenv(s_lang);
    _putenv(s_language);
    #else
    putenv(s_lang);
    putenv(s_language);
    #endif

    // General initialization
    setlocale(LC_ALL, "");

    // Really needed for pugixml... otherwise parsing issues with floating points
    setlocale(LC_NUMERIC, "C");

    // Set text domain
    bindtextdomain("eev", "res/po/");
    textdomain("eev");
}

void internationalization::close()
{
    // Free memory
    if (s_lang != nullptr) delete[] s_lang;
    if (s_language != nullptr) delete[] s_language;

    s_lang = nullptr;
    s_language = nullptr;
}
