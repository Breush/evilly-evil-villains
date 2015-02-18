#include "core/gettext.hpp"

#include <iostream>

int main(void)
{	
	internationalization::init("fr_FR");
	if (_("Monsters") != L"Monstres") {
		std::wcout << L"French translation for \"Monsters\" differs from expected: ";
		std::wcout << _("Monsters") << " != " << L"Monstres" << std::endl;
		EXIT_FAILURE;
	}
    
	return EXIT_SUCCESS;
}

