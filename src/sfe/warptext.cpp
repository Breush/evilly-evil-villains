#include "sfe/warptext.hpp"

#include "tools/tools.hpp"

#include <sstream>

using namespace sfe;

//-------------------//
//----- Fitting -----//

void WarpText::setWarpString(std::wstring string)
{
    // Copy the original string
    m_warpString = string;
    rewarp();
}

void WarpText::fitWidth(float inFitWidth)
{
    m_fitWidth = inFitWidth;
    rewarp();
}

void WarpText::rewarp()
{
    returnif (m_fitWidth < 0.f);

    // Word warp - greedy algorithm
    std::wistringstream str(m_warpString);
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
