#pragma once

#include "context/command.hpp"
#include "tools/string.hpp"

#include <SFML/System/NonCopyable.hpp>

#include <queue>
#include <vector>

namespace context
{
    //! Command queue and broadcaster.

    class Commander final : private sf::NonCopyable
    {
        friend class Commandable;

    public:

        //----------------//
        //! @name Routine
        //! @{

        //! Empty the queue and broadcast the commands.
        void update(const sf::Time& dt);

        //! @}

        //--------------//
        //! @name Queue
        //! @{

        //! Add a command to the queue.
        void push(const Command& command);

        //! Get last command.
        Command pop();

        //! @}

        //--------------------//
        //! @name Interpreter
        //! @{

        //! Interpret the command line into commands.
        Command interpret(const std::wstring& commandLine);

        //! @}

    protected:

        //-----------------//
        //! @name Register
        //! @{

        //! Add the commandable to the update list, called directly by the commandable.
        void add(Commandable* pCommandable);

        //! Remove the commandable from the update list, called directly by the commandable.
        void remove(Commandable* pCommandable);

        //! @}

    private:

        std::queue<Command> m_commandQueue;         //!< The command queue.

        // TODO Better have separate lists of commandables, given their category.
        std::vector<Commandable*> m_commandables;   //!< The commandables to call on update.
    };
}
