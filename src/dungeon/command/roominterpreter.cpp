#include "dungeon/command/roominterpreter.hpp"

#include "context/logger.hpp"
#include "dungeon/inter.hpp"

using namespace dungeon;

RoomInterpreter::RoomInterpreter(Inter& inter)
    : m_inter(inter)
    , m_facilitiesInterpreter(m_inter)
    , m_trapInterpreter(m_inter)
{
}

//----------------//
//----- Data -----//

void RoomInterpreter::roomSet(const RoomCoords& coords)
{
    m_roomCoords = coords;
}

//-----------------------//
//----- Interpreter -----//

void RoomInterpreter::interpret(std::vector<context::Command>& commands, std::vector<std::wstring>& tokens)
{
    std::wstring logMessage = L"> [room] ";
    auto nTokens = tokens.size();

    if (nTokens >= 1u) {
        if (tokens[0u] == L"construct") {
            logMessage += L"Constructing";
            bool free = (nTokens >= 2u) && (tokens[1u] == L"free");
            m_inter.constructRoom(m_roomCoords, free);
            goto logging;
        }
        else if (tokens[0u] == L"destroy") {
            logMessage += L"Destroying";
            bool loss = (nTokens >= 2u) && (tokens[1u] == L"loss");
            m_inter.destroyRoom(m_roomCoords, loss);
            goto logging;
        }
        else if (tokens[0u] == L"facilities") {
            logMessage += L"Accessing facilities\n";
            tokens.erase(std::begin(tokens));
            m_facilitiesInterpreter.roomSet(m_roomCoords);
            m_facilitiesInterpreter.interpret(commands, tokens);
            goto logging;
        }
        else if (tokens[0u] == L"trap") {
            logMessage += L"Accessing trap\n";
            tokens.erase(std::begin(tokens));
            m_trapInterpreter.roomSet(m_roomCoords);
            m_trapInterpreter.interpret(commands, tokens);
            goto logging;
        }
    }

    if (nTokens >= 2u) {
        if (tokens[0u] == L"push") {
            logMessage += L"Pushing room " + tokens[1u];
            auto direction = directionFromString(tokens[1u]);
            auto animationTime = (nTokens >= 3u)? to<uint>(tokens[2u]) : 250u;
            m_inter.pushRoom(m_roomCoords, direction, animationTime);
        }
    }

    logMessage += L"Unable to interpret command: " + join(tokens, std::wstring(L" "));

    // Generate log
    logging:
    context::addCommandLog(commands, logMessage);
}

void RoomInterpreter::autoComplete(std::vector<std::wstring>& tokens, const std::function<void(const std::wstring&)>& checkAdd) const
{
    auto nTokens = tokens.size();

    if (nTokens == 0u) {
        checkAdd(L"construct");
        checkAdd(L"destroy");
        checkAdd(L"facilities");
        checkAdd(L"trap");
        checkAdd(L"push");
        return;
    }
    else if (nTokens == 1u) {
        if (tokens[0u] == L"push") {
            checkAdd(L"north");
            checkAdd(L"south");
            checkAdd(L"west");
            checkAdd(L"east");
            return;
        }
    }

    if (nTokens >= 1u) {
        if (tokens[0u] == L"trap") {
            tokens.erase(std::begin(tokens));
            m_trapInterpreter.autoComplete(tokens, checkAdd);
            return;
        }
        else if (tokens[0u] == L"facilities") {
            tokens.erase(std::begin(tokens));
            m_facilitiesInterpreter.autoComplete(tokens, checkAdd);
            return;
        }
    }
}
