#pragma once

#include "context/command.hpp"
#include "dungeon/command/roominterpreter.hpp"

namespace dungeon
{
    //! Interprets a multiple rooms-wide command.

    class RoomsInterpreter
    {
    public:

        //! Constructor.
        RoomsInterpreter(Inter& inter);

        //! Default destructor.
        ~RoomsInterpreter() = default;

        //-------------//
        //! @name Data
        //! @{

        //! Remove all known rooms.
        void roomsClear();

        //! Add a room to the list.
        void roomsAdd(const RoomCoords& coords);

        //! @}

        //--------------------//
        //! @name Interpreter
        //! @{

        //! Interpret a command line.
        void interpret(std::vector<context::Command>& commands, std::vector<std::wstring>& tokens);

        //! Tries to auto-complete.
        void autoComplete(std::vector<std::wstring>& tokens, const std::function<void(const std::wstring&)>& checkAdd) const;

        //! @}

    private:

        Inter& m_inter;     //!< Reference to the inter.

        std::vector<RoomInterpreter> m_roomInterpreters;    //!< All the room interpreters.
    };
}
