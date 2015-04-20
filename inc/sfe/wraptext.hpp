#pragma once

#include "tools/string.hpp"

#include <SFML/Graphics/Text.hpp>

namespace sfe
{
    // TODO This could be a good idea to make this class containing a sf::Text
    // So that rewraps will be automated.

    //! A classic sf::Text that can wrap itself to fit a given width.
    /*!
     *  One should use setWrapString() instead of setString()
     *  and define the maximum width with fitWidth().
     *  If you modify how the text look, call rewrap().
     */

    class WrapText final : public sf::Text
    {
    public:

        //! Default constructor.
        WrapText() = default;

        //! Default destructor.
        ~WrapText() = default;

        //----------------//
        //! @name Fitting
        //! @{

        //! Set the string to wrap.
        void setWrapString(std::wstring string);

        //! Set the width within which the text should fit.
        void fitWidth(float inFitWidth);

        //! Ask for a rewrapping of the current wrap string.
        void rewrap();

        //! @}

    private:

        std::wstring m_wrapString;    //!< The original
        float m_fitWidth = -1.f;    //!< The width to fit.
    };
}
