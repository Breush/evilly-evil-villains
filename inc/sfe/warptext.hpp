#pragma once

#include "tools/string.hpp"

#include <SFML/Graphics/Text.hpp>

namespace sfe
{
    // TODO This could be a good idea to make this class containing a sf::Text
    // So that rewarps() will be automated.

    //! A classic sf::Text that can wrap itself to fit a given width.
    /*!
     *  One should use setWarpString() instead of setString()
     *  and define the maximum width with fitWidth().
     *  If you modify how the text look, call rewarp().
     */

    class WarpText final : public sf::Text
    {
    public:

        //! Default constructor.
        WarpText() = default;

        //! Default destructor.
        ~WarpText() = default;

        //----------------//
        //! @name Fitting
        //! @{

        //! Set the string to warp.
        void setWarpString(std::wstring string);

        //! Set the width within which the text should fit.
        void fitWidth(float inFitWidth);

        //! Ask for a rewarping of the current warp string.
        void rewarp();

        //! @}

    private:

        std::wstring m_warpString;    //!< The original
        float m_fitWidth = -1.f;    //!< The width to fit.
    };
}
