#include "sfe/wraptext.hpp"

#include "tools/tools.hpp"

#include <sstream>

using namespace sfe;

//-------------------//
//----- Fitting -----//

void WrapText::setWrapString(std::wstring string)
{
    // Copy the original string
    m_wrapString = string;
    rewrap();
}

void WrapText::fitWidth(float inFitWidth)
{
    m_fitWidth = inFitWidth;
    rewrap();
}

void WrapText::rewrap()
{
    returnif (m_fitWidth < 0.f);

    // Word wrap - greedy algorithm
    std::wistringstream str(m_wrapString);
    std::wstring prevString, string;
    std::wstring word;

    while (str >> word) {
        string += word;
        setString(string);

        if (boundsSize(*this).x > m_fitWidth) {
            string = prevString + L'\n' + word;
            setString(string);
        }

        prevString = string;
        string += L' ';
    }
}
