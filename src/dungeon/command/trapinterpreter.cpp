#include "dungeon/command/trapinterpreter.hpp"

#include "context/logger.hpp"
#include "dungeon/inter.hpp"

using namespace dungeon;

TrapInterpreter::TrapInterpreter(Inter& inter)
    : m_inter(inter)
{
}

//----------------//
//----- Data -----//

void TrapInterpreter::roomSet(const RoomCoords& coords)
{
    m_roomCoords = coords;
}

//-----------------------//
//----- Interpreter -----//

context::CommandPtr TrapInterpreter::interpret(std::vector<std::wstring>& tokens, std::wstring& logMessage)
{
    logMessage += L"> [trap] ";
    auto nTokens = tokens.size();

    if (nTokens >= 1u) {
        if (tokens[0u] == L"remove") {
            logMessage += L"Removing trap";
            bool loss = (nTokens >= 2u) && (tokens[1u] == L"loss");
            m_inter.removeRoomTrap(m_roomCoords, loss);
            goto logging;
        }
    }

    if (nTokens >= 2u) {
        if (tokens[0u] == L"set") {
            logMessage += L"Setting trap " + tokens[1u];
            bool free = (nTokens >= 3u) && (tokens[2u] == L"free");
            m_inter.setRoomTrap(m_roomCoords, tokens[1u], free);
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

void TrapInterpreter::autoComplete(std::vector<std::wstring>& tokens, const std::function<void(const std::wstring&)>& checkAdd) const
{
    auto nTokens = tokens.size();

    if (nTokens == 0u) {
        checkAdd(L"remove");
        checkAdd(L"set");
        return;
    }
    else if (nTokens == 1u) {
        if (tokens[0u] == L"remove") {
            checkAdd(L"loss");
            return;
        }
        else if (tokens[0u] == L"set") {
            for (const auto& trapInfoPair : m_inter.trapsDB().get())
                checkAdd(trapInfoPair.first);
            return;
        }
    }
    else if (nTokens == 2u) {
        if (tokens[0u] == L"set") {
            checkAdd(L"free");
            return;
        }
    }
}
