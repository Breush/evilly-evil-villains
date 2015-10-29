#include "core/gettext.hpp"

#include <iostream>

int main(void)
{
    i18n::init("fr_FR");

    if (_("Monsters") != L"Monstres") {
        std::wcout << L"French translation for \"Monsters\" differs from expected:" << std::endl;
        std::wcout << _("Monsters") << L" != Monstres" << std::endl;
        return EXIT_FAILURE;
    }

    i18n::close();

    return EXIT_SUCCESS;
}

