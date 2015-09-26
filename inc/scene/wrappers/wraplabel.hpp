#pragma once

#include "scene/wrappers/tlabel.hpp"
#include "sfe/twraptext.hpp"

namespace scene
{
    //! WrapLabel, a text fitting a specific width.
    template<class Text_t>
    class WrapLabel final : public TLabel<sfe::WrapText<Text_t>>
    {
        using baseClass = TLabel<sfe::WrapText<Text_t>>;

    public:

        //----------------//
        //! @name Fitting
        //! @{

        //! Set the width within which the text should fit.
        inline void fitWidth(float inFitWidth)
        {
            baseClass::text().fitWidth(inFitWidth);
            baseClass::updateSize();
        }

        //! @}
    };
}
