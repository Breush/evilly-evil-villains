#pragma once

#include "context/command.hpp"
#include "dungeon/command/roomsinterpreter.hpp"

namespace dungeon
{
    //! The interpreter of the dungeon.

    class Interpreter final : private context::Interpreter
    {
        using baseClass = Interpreter;

        std::wstring interpreterKey() const final { return L"dungeon"; }

    public:

        //! Constructor.
        Interpreter(Inter* inter);

        //! Default destructor.
        ~Interpreter() = default;

    protected:

        //--------------------//
        //! @name Interpreter
        //! @{

        void interpret(std::vector<context::Command>& commands, std::vector<std::wstring>& tokens) final;
        void autoComplete(std::vector<std::wstring>& possibilities,
                          const std::vector<std::wstring>& tokens, const std::wstring& lastToken) final;

        //! @}

    private:

        Inter& m_inter;     //!< Reference to the inter.

        RoomsInterpreter m_roomsInterpreter;    //!< Rooms interpreter.
        RoomInterpreter m_roomInterpreter;      //!< Room interpreter.
    };
}
