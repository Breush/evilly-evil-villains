#include "dungeon/data.hpp"
#include "tools/string.hpp"

#include <iostream>
#include <fstream>

int main(void)
{
    dungeon::Data data;

    // Loading and saving
    auto loadedFilename = data.load(L"../tests/data/test-dungeon-data/");
    auto savedFilename = data.save(L"../tests/data/test-dungeon-data/saved-");

    // Files should be identical
    std::wifstream loaded(toString(loadedFilename));
    std::wifstream saved(toString(savedFilename));

    if (!loaded.is_open() || !saved.is_open()) {
        std::cerr << "Cannot load files." << std::endl;
        return EXIT_FAILURE;
    }

    std::wstring loadedString;
    std::wstring savedString;

    uint lineNumber = 1u;
    while (getline(loaded, loadedString) && getline(saved, savedString)) {
        if (loadedString != savedString) {
            std::wcout << L"Saved file is different from loaded file at line " << lineNumber << L"." << std::endl;
            std::wcout << L"Load: " << loadedString << std::endl;
            std::wcout << L"Save: " << savedString << std::endl;
            return EXIT_FAILURE;
        }

        ++lineNumber;
    }

    return EXIT_SUCCESS;
}
