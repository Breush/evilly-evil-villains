#pragma once

#include "context/command.hpp"
#include "dungeon/command/facilityinterpreter.hpp"

#include <functional>

namespace dungeon
{
    // Forward declarations

    class Inter;
    class Facility;

    //! Interprets a room-wide command.

    class FacilityInterpreter
    {
    public:

        //! Constructor.
        FacilityInterpreter(Inter& inter);

        //! Default destructor.
        ~FacilityInterpreter() = default;

        //-------------//
        //! @name Data
        //! @{

        //! Set the facility.
        void facilitySet(Facility* pFacility);

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

        Inter& m_inter;         //!< Reference to the inter.

        Facility* m_pFacility;  //!< Located facility reference.
    };
}
