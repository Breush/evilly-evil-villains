#pragma once

#include "tools/string.hpp"

#include <libintl.h>
#include <cstdlib>

namespace i18n
{
    //! To be called when changing language.
    void init(const std::string& language);

    //! To be called once when closing the program.
    void close();

    //! A language info.
    struct LanguageInfo
    {
        std::wstring code;      //!< i.e. fr_FR
        std::wstring name;      //!< i.e. Français (France)
    };

    //! Procedure to init the languages list.
    void initLanguagesList();

    //! List the available languages.
    const std::vector<LanguageInfo>& languagesList();

    //! Find an available language index from its code.
    uint languageIndexFromCode(const std::wstring& code);
}

#define _(STRING) toWString(std::string(gettext(STRING)))
