#pragma once

#include "context/command.hpp"
#include "dungeon/structs/room.hpp"
#include "dungeon/command/facilityinterpreter.hpp"

#include <functional>

namespace dungeon
{
    // Forward declarations

    class Inter;

    //! Interprets a room-wide command.

    class TrapInterpreter
    {
    public:

        //! Constructor.
        TrapInterpreter(Inter& inter);

        //! Default destructor.
        ~TrapInterpreter() = default;

        //-------------//
        //! @name Data
        //! @{

        //! Set the room from its coords.
        void roomSet(const RoomCoords& coords);

        //! @}

        //--------------------//
        //! @name Interpreter
        //! @{

        //! Interpret a command line.
        void interpret(std::vector<context::Command>& commands, std::vector<std::wstring>& tokens);

        //! Tries to auto-complete the lastToken.
        void autoComplete(std::vector<std::wstring>& tokens, const std::function<void(const std::wstring&)>& checkAdd) const;

        //! @}

    private:

        Inter& m_inter;         //!< Reference to the inter.

        RoomCoords m_roomCoords;    //!< The room coords.
    };
}
