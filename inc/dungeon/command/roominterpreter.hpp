#pragma once

#include "dungeon/command/facilitiesinterpreter.hpp"
#include "dungeon/command/trapinterpreter.hpp"

namespace dungeon
{
    //! Interprets a room-wide command.
    // TODO This could become the key part where room commands occurs,
    // dungeon::Inter having no interface to do things like constructRoom()

    class RoomInterpreter
    {
    public:

        //! Constructor.
        RoomInterpreter(Inter& inter);

        //! Default destructor.
        ~RoomInterpreter() = default;

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
        context::CommandPtr interpret(std::vector<std::wstring>& tokens, std::wstring& logMessage);

        //! Tries to auto-complete.
        void autoComplete(std::vector<std::wstring>& tokens, const std::function<void(const std::wstring&)>& checkAdd) const;

        //! @}

    private:

        Inter& m_inter;     //!< Reference to the inter.

        RoomCoords m_roomCoords;    //!< The room coords.

        FacilitiesInterpreter m_facilitiesInterpreter;  //!< Facilities interpreter.
        TrapInterpreter m_trapInterpreter;              //!< Trap interpreter.
    };
}
