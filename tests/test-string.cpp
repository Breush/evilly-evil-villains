#include "core/gettext.hpp"

#include <iostream>

int main(void)
{
    std::string str;
    std::wstring wstr;

    // French, to check that characters like 'É' are considered alphanum
    internationalization::init("fr_FR");

    //-------//
    // Lower //
    
    // String
    str = "FUnNY S'TriNG!";
    if (toLowercase(str) != "funny s'tring!") {
        std::cerr << "String to lowercase differs from expected:" << std::endl;
        std::cerr << str << " != funny s'tring!" << std::endl;
        return EXIT_FAILURE;
    }

    // Wide string
    wstr = L"FÜnNY S'TrîNG!";
    if (toLowercase(wstr) != L"fünny s'trîng!") {
        std::wcout << L"String to lowercase differs from expected:" << std::endl;
        std::wcout << wstr << L" != fünny s'trîng!" << std::endl;
        return EXIT_FAILURE;
    }

    //-------//
    // Upper //
    
    // String
    str = "FUnNY S'TriNG!";
    if (toUppercase(str) != "FUNNY S'TRING!") {
        std::cerr << "String to uppercase differs from expected:" << std::endl;
        std::cerr << str << " != FUNNY S'TRING!" << std::endl;
        return EXIT_FAILURE;
    }

    // Wide string
    wstr = L"FÜnNY S'TrîNG!";
    if (toUppercase(wstr) != L"FÜNNY S'TRÎNG!") {
        std::wcout << L"String to uppercase differs from expected:" << std::endl;
        std::wcout << wstr << L" != FÜNNY S'TRÎNG!" << std::endl;
        return EXIT_FAILURE;
    }

    //---------//
    // Special //
    
    // String
    str = "FUn-NY... S'TriNG!";
    if (filterSpecial(str, false) != "FUn_NY____S_TriNG_") {
        std::cerr << "String special filter differs from expected:" << std::endl;
        std::cerr << str << " != FUn_NY____S_TriNG_" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Wide string
    wstr = L"FÜn-NY... S'TrîNG!";
    if (filterSpecial(wstr, false) != L"FÜn_NY____S_TrîNG_") {
        std::wcout << L"String special filter differs from expected:" << std::endl;
        std::wcout << wstr << L" != FÜn_NY____S_TrîNG_" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

