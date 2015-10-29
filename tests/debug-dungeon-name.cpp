// Unit test to debug #61 not align titles in dungeon::Summary.
// Appears that dungeon name might not be parsed correctly.

#include "dungeon/data.hpp"
#include "tools/string.hpp"
#include "core/gettext.hpp"

#include <iostream>
#include <fstream>

int main(void)
{
    i18n::init("en_EN");
    
    dungeon::Data data;
    data.load(L"../tests/data/debug-dungeon-name/");
   
    std::wcout << L"Name: " << data.name() << std::endl;
    
    if (data.name() != L"¥£€$¢₡₢₣₤₥₦₧₨₩₪₫₭₮₯₹")
        return EXIT_FAILURE;
        
    i18n::close();

    return EXIT_SUCCESS;
}
