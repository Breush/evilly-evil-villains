#include "core/gettext.hpp"

#include <iostream>

int main(void)
{
    std::string str;
    std::wstring wstr;

    // French, to check that characters like 'É' are considered alphanum
    internationalization::init("fr_FR");

    //-------------------------//
    // String <-> WSide string //

    // String
    str = "FUnNY S'TriNG!";
    if ((wstr = toWString(str)) != L"FUnNY S'TriNG!") {
        std::wcout << L"String to Wide string differs from expected:" << std::endl;
        std::wcout << wstr << L" != FUnNY S'TriNG!" << std::endl;
        return EXIT_FAILURE;
    }

    // Wide string
    wstr = L"FUnNY S'TriNG!";
    if ((str = toString(wstr)) != "FUnNY S'TriNG!") {
        std::cerr << "Wide string to String differs from expected:" << std::endl;
        std::cerr << str << " != FUnNY S'TriNG!" << std::endl;
        return EXIT_FAILURE;
    }

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

    //--------------//
    // Manipulation //

    if (join({"a","b","c"},std::string(",")) != "a,b,c")    return EXIT_FAILURE;
    if (join({""},std::string(",")) != "")                  return EXIT_FAILURE;

    std::cerr << prefix(std::string("aaa"), std::string("aab")) << std::endl;

    if (prefix(std::string("aaa"), std::string("aab")) != "aa") return EXIT_FAILURE;
    if (prefix(std::string("aaa"), std::string("bab")) != "")   return EXIT_FAILURE;

    internationalization::close();

    return EXIT_SUCCESS;
}

