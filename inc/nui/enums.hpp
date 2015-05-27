#pragma once

namespace nui
{
    //! Define relative position.
    enum class Align
    {
        STANDARD,   //!< Left or Top.
        CENTER,     //!< Center.
        OPPOSITE,   //!< RIght or Bottom.
    };

    //! Define how a column/row adapt to content.
    enum class Adapt
    {
        FILL,   //!< Share the left space with other fill.
        FIT,    //!< Fit the size to the max of children.
        FIXED,  //!< The size is fixed.
    };
}

