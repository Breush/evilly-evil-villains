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

void FacilityInterpreter::interpret(std::vector<context::Command>& commands, std::vector<std::wstring>& tokens)
{
    std::wstring logMessage = L"> [facility] ";
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
            // TODO [easyfix] Should be "add", not "set" (update API doc too)
            if (tokens[1u] == L"set") {
                logMessage += L"Setting link to room " + tokens[2u] + L"/" + tokens[3u];
                RoomCoords linkCoords{to<uint8>(tokens[2u]), to<uint8>(tokens[3u])};
                uint8 id = (nTokens >= 6u && tokens[4u] == L"id")? to<uint8>(tokens[5u]) : 0xFF;
                m_inter.facilityLinksAdd(m_pFacility->facilityInfo(), linkCoords, id);
                goto logging;
            }
        }
    }

    logMessage += L"Unable to interpret command: " + join(tokens, std::wstring(L" "));

    // Generate log
    logging:
    context::addCommandLog(commands, logMessage);
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
