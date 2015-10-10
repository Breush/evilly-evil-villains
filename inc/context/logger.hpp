#pragma once

#include "context/command.hpp"

namespace context
{
    //! A commandable that log things.

    class Logger : public Commandable
    {
    public:

        //-----------------//
        //! @name Commands
        //! @{

        //! Command result.
        virtual void commandLog(const std::wstring& message) = 0;

        //! @}
    };
}
