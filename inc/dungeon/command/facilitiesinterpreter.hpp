#pragma once

#include "dungeon/structs/room.hpp"
#include "dungeon/command/facilityinterpreter.hpp"

namespace dungeon
{
    //! Interprets a room-wide command.

    class FacilitiesInterpreter
    {
    public:

        //! Constructor.
        FacilitiesInterpreter(Inter& inter);

        //! Default destructor.
        ~FacilitiesInterpreter() = default;

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

        //! Tries to auto-complete.
        void autoComplete(std::vector<std::wstring>& tokens, const std::function<void(const std::wstring&)>& checkAdd) const;

        //! @}

    private:

        Inter& m_inter;     //!< Reference to the inter.

        RoomCoords m_roomCoords;    //!< The room coords.

        FacilityInterpreter m_facilityInterpreter;  //!< Facility interpreter.
    };
}
