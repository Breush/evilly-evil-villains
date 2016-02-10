#include "dungeon/command/facilitiesinterpreter.hpp"

#include "context/logger.hpp"
#include "dungeon/inter.hpp"

using namespace dungeon;

FacilitiesInterpreter::FacilitiesInterpreter(Inter& inter)
    : m_inter(inter)
    , m_facilityInterpreter(m_inter)
{
}

//----------------//
//----- Data -----//

void FacilitiesInterpreter::roomSet(const RoomCoords& coords)
{
    m_roomCoords = coords;
}

//-----------------------//
//----- Interpreter -----//

context::CommandPtr FacilitiesInterpreter::interpret(std::vector<std::wstring>& tokens, std::wstring& logMessage)
{
    logMessage += L"> [facilities] ";
    auto nTokens = tokens.size();

    if (nTokens >= 2u) {
        if (tokens[0u] == L"create") {
            logMessage += L"Creating facility " + tokens[1u];
            bool free = (nTokens >= 3u) && (tokens[2u] == L"free");
            m_inter.facilitiesCreate(m_roomCoords, tokens[1u], free);
            goto logging;
        }
        else if (tokens[0u] == L"find") {
            logMessage += L"Accessing facility " + tokens[1u];
            auto pFacility = m_inter.facilitiesFind(m_roomCoords, tokens[1u]);
            if (pFacility == nullptr) {
                logMessage += L" -> not found";
                goto logging;
            }

            logMessage += L"\n";
            tokens.erase(std::begin(tokens), std::begin(tokens) + 2u);
            m_facilityInterpreter.facilitySet(pFacility);
            return m_facilityInterpreter.interpret(tokens, logMessage);
        }
        else if (tokens[0u] == L"remove") {
            logMessage += L"Removing facility " + tokens[1u];
            bool loss = (nTokens >= 3u) && (tokens[2u] == L"loss");
            m_inter.facilitiesRemove(m_roomCoords, tokens[1u], loss);
            goto logging;
        }
    }

    logMessage += L"Unable to interpret command: " + join(tokens, std::wstring(L" "));

    // Generate log
    logging:
    auto pCommand = std::make_unique<context::Command>();
    context::setCommandLog(*pCommand, logMessage);
    return std::move(pCommand);
}

void FacilitiesInterpreter::autoComplete(std::vector<std::wstring>& tokens, const std::function<void(const std::wstring&)>& checkAdd) const
{
    auto nTokens = tokens.size();

    if (nTokens == 0u) {
        checkAdd(L"create");
        checkAdd(L"find");
        checkAdd(L"remove");
        return;
    }
    else if (nTokens == 1u) {
        if (tokens[0u] == L"create" || tokens[0u] == L"find" || tokens[0u] == L"remove") {
            for (const auto& facilityInfoPair : m_inter.facilitiesDB().get())
                checkAdd(facilityInfoPair.first);
            return;
        }
    }
    else if (nTokens == 2u) {
        if (tokens[0u] == L"create") {
            checkAdd(L"free");
            return;
        }
        else if (tokens[0u] == L"remove") {
            checkAdd(L"loss");
            return;
        }
    }

    if (nTokens >= 2u) {
        if (tokens[0u] == L"find") {
            tokens.erase(std::begin(tokens), std::begin(tokens) + 2u);
            m_facilityInterpreter.autoComplete(tokens, checkAdd);
        }
    }
}
