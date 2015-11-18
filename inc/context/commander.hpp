#pragma once

#include "context/command.hpp"

#include <SFML/System/NonCopyable.hpp>

#include <queue>

namespace context
{
    //! Command queue and broadcaster.

    class Commander final : private sf::NonCopyable
    {
        friend class Commandable;
        friend class Interpreter;

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

        //! Attempt to auto-complete the command line.
        //! @return The command line completed or the original one.
        std::wstring autoComplete(std::wstring commandLine);
        //! @}

    protected:

        //-----------------//
        //! @name Register
        //! @{

        //! Add the commandable to the update list, called directly by the commandable.
        void add(Commandable* pCommandable);

        //! Remove the commandable from the update list, called directly by the commandable.
        void remove(Commandable* pCommandable);

        //! Add the interpreter to the list, called directly by the interpreter.
        void add(Interpreter* pInterpreter);

        //! Remove the interpreter from the list, called directly by the interpreter.
        void remove(Interpreter* pInterpreter);

        //! @}

    private:

        std::queue<Command> m_commandQueue;         //!< The command queue.
        std::vector<Commandable*> m_commandables;   //!< The commandables to call on update.
        std::vector<Interpreter*> m_interpreters;   //!< The interpreter to call when a command line comes.
    };
}
