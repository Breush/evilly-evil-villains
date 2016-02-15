#include "dungeon/command/facilityinterpreter.hpp"

#include "context/logger.hpp"
#include "dungeon/inter.hpp"

using namespace dungeon;

FacilityInterpreter::FacilityInterpreter(Inter& inter)
    : m_inter(inter)
{
}

//----------------//
//----- Data -----//

void FacilityInterpreter::facilitySet(Facility* pFacility)
{
    m_pFacility = pFacility;
}

//-----------------------//
//----- Interpreter -----//

context::CommandPtr FacilityInterpreter::interpret(std::vector<std::wstring>& tokens, std::wstring& logMessage)
{
    logMessage += L"> [facility] ";
    auto nTokens = tokens.size();

    // Note: If ai or link become way bigger,
    // think to split them to their own interpreter.

    if (nTokens >= 2u) {
        if (tokens[0u] == L"ai") {
            logMessage += L"Executing AI: " + tokens[1u];
            auto luaKey = toString(tokens[1u]);
            m_pFacility->lua()(luaKey.c_str());
            goto logging;
        }
    }

    if (nTokens >= 4u) {
        if (tokens[0u] == L"link") {
            // TODO Link Should be add
            if (tokens[1u] == L"set") {
                logMessage += L"Setting link to room " + tokens[2u] + L"/" + tokens[3u];
                RoomCoords linkCoords{to<uint8>(tokens[2u]), to<uint8>(tokens[3u])};
                m_inter.facilityLinksAdd(m_pFacility->coords(), m_pFacility->edata().type(), linkCoords);
                goto logging;
            }
        }
    }

    logMessage += L"Unable to interpret command: " + join(tokens, std::wstring(L" "));

    // Generate log
    logging:
    auto pCommand = std::make_unique<context::Command>();
    context::setCommandLog(*pCommand, logMessage);
    return std::move(pCommand);
}

void FacilityInterpreter::autoComplete(std::vector<std::wstring>& tokens, const std::function<void(const std::wstring&)>& checkAdd) const
{
    auto nTokens = tokens.size();

    if (nTokens == 0u) {
        checkAdd(L"ai");
        checkAdd(L"link");
        return;
    }
    else if (nTokens == 1u) {
        if (tokens[0u] == L"link") {
            checkAdd(L"set");
            return;
        }
    }
}
