#include "sfe/wraptext.hpp"

#include "tools/tools.hpp"
#include "tools/int.hpp"

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
    std::wstring prevString, string;
    uint i = 0u;

    while (i != m_wrapString.size()) {
        std::wstring word;
        wchar_t c;

        // Get word
        while (i < m_wrapString.size() && !iswspace(c = m_wrapString[i])) {
            word += c;
            ++i;
        }

        // Update string, and add a Return character
        // between last words if we went to far
        string += word;
        setString(string);

        if (boundsSize(*this).x > m_fitWidth) {
            string = prevString + L'\n' + word;
            setString(string);
        }

        prevString = string;

        // Add separators
        while (i < m_wrapString.size() && iswspace(c = m_wrapString[i])) {
            string += c;
            ++i;
        }
    }
}
