#include "dungeon/command/roomsinterpreter.hpp"

#include "context/logger.hpp"

using namespace dungeon;

RoomsInterpreter::RoomsInterpreter(Inter& inter)
    : m_inter(inter)
{
}

//----------------//
//----- Data -----//

void RoomsInterpreter::roomsClear()
{
    m_roomInterpreters.clear();
}

void RoomsInterpreter::roomsAdd(const RoomCoords& coords)
{
    m_roomInterpreters.emplace_back(m_inter);
    m_roomInterpreters.back().roomSet(coords);
}

//-----------------------//
//----- Interpreter -----//

context::CommandPtr RoomsInterpreter::interpret(std::vector<std::wstring>& tokens, std::wstring& logMessage)
{
    // TODO command keep to filter the rooms

    logMessage += L"> [rooms] ";
    auto nTokens = tokens.size();

    if (nTokens >= 1u) {
        if (tokens[0u] == L"foreach") {
            // Note: The generated commands are dropped because it is just message log
            // And we drop the logMessage, because otherwise there can be a seg fault for too long message
            // OPTIM There should be a way execute the command on each room, but interpreting it only once...
            tokens.erase(std::begin(tokens));
            logMessage += L"For each room... ";
            for (auto& roomInterpreter : m_roomInterpreters) {
                auto newTokens = tokens;
                std::wstring dropLogMessage;
                roomInterpreter.interpret(newTokens, dropLogMessage);
            }
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

void RoomsInterpreter::autoComplete(std::vector<std::wstring>& tokens, const std::function<void(const std::wstring&)>& checkAdd) const
{
    auto nTokens = tokens.size();

    if (nTokens == 0u) {
        checkAdd(L"foreach");
        return;
    }

    if (nTokens >= 1u) {
        if (tokens[0u] == L"foreach") {
            tokens.erase(std::begin(tokens));
            RoomInterpreter(m_inter).autoComplete(tokens, checkAdd);
            return;
        }
    }
}
