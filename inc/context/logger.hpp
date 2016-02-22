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

        inline Command::Category category() const final { return Command::Category::LOG; }

        //! Command result.
        virtual void commandLog(const std::wstring& message) = 0;

        //! @}
    };

    //! Utility function to set up a command as a log.
    inline Command& addCommandLog(std::vector<Command>& commands, const std::wstring& message)
    {
        commands.emplace_back();
        auto& command = commands.back();
        command.category = Command::Category::LOG;
        command.action = [message] (Commandable& commandable, const sf::Time&)
            { commandable.as<Logger>().commandLog(message); };
        return command;
    }
}
