// MinGW does not declare _putenv() because of -std=c++11,
// this fix enables it.
#ifdef __WIN32__
    #undef __STRICT_ANSI__
#endif

#include "core/gettext.hpp"

#include "core/debug.hpp"
#include "tools/filesystem.hpp"
#include "tools/tools.hpp"

#include <pugixml/pugixml.hpp>

#include <cstdlib>
#include <cstring>
#include <locale>

//----------------------------//
//----- Static variables -----//

static char* s_lang = nullptr;
static char* s_language = nullptr;
static std::string s_loadedLanguage;

static std::vector<i18n::LanguageInfo> s_languagesList;

//---------------------------//
//----- Internalization -----//

void i18n::init(const std::string& language)
{
    returnif (s_loadedLanguage == language);

    mdebug_core_1("Setting language to " << language);
    s_loadedLanguage = language;

    // Free memory if any
    close();

    // Force a specific language to the environment
    auto sLang = "LANG=" + language + ".utf8";
    auto sLanguage = "LANGUAGE=" + language + ".utf8";

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
    setlocale(LC_MESSAGES, "");

    // Really needed for pugixml... otherwise parsing issues with floating points
    setlocale(LC_NUMERIC, "C");

    // Set text domain
    bindtextdomain("eev", "res/core/langs/");
    textdomain("eev");
}

void i18n::close()
{
    // Free memory
    if (s_lang != nullptr) delete[] s_lang;
    if (s_language != nullptr) delete[] s_language;

    s_lang = nullptr;
    s_language = nullptr;
}

//--------------------------//
//----- Languages list -----//

void i18n::initLanguagesList()
{
    s_languagesList.clear();

    for (auto& fileInfo : listFiles("res/core/langs")) {
        if (!fileInfo.isDirectory) continue;

        pugi::xml_document doc;
        if (!doc.load_file((fileInfo.fullName + "/info.xml").c_str())) continue;
        const auto& translationNode = doc.child(L"translation");
        const auto& languageNode = translationNode.child(L"language");

        i18n::LanguageInfo language;
        language.code = languageNode.attribute(L"code").as_string();
        language.name = languageNode.attribute(L"name").as_string();
        s_languagesList.emplace_back(std::move(language));
    }
}

const std::vector<i18n::LanguageInfo>& i18n::languagesList()
{
    return s_languagesList;
}

uint i18n::languageIndexFromCode(const std::wstring& code)
{
    auto languagesCount = s_languagesList.size();
    for (uint i = 0u ; i < languagesCount; ++i)
        if (s_languagesList[i].code == code)
            return i;

    return -1u;
}
