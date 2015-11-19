#include "dungeon/commandable.hpp"

#include "dungeon/inter.hpp"
#include "context/logger.hpp"

using namespace dungeon;

Commandable::Commandable(Inter* inter)
    : m_inter(*inter)
{
}

//-----------------------//
//----- Interpreter -----//

context::Command Commandable::interpret(const std::vector<std::wstring>& tokens)
{
    std::wstring logMessage = L"> [dungeon] Unable to interpret command";
    auto nTokens = tokens.size();

    if (nTokens >= 4u) {
        // Construct room
        if (tokens[0u] == L"construct" && tokens[1u] == L"room") {
            logMessage = L"> [dungeon] Constructing room " + tokens[2u] + L"/" + tokens[3u];
            bool free = (nTokens >= 5u) && (tokens[4u] == L"free");
            m_inter.constructRoom({to<uint>(tokens[2u]), to<uint>(tokens[3u])}, free);
        }
    }

    // Generate log
    context::Command command;
    return context::setCommandLog(command, logMessage);
}

void Commandable::autoComplete(std::vector<std::wstring>& possibilities,
                               const std::vector<std::wstring>& tokens, const std::wstring& lastToken)
{
    // TODO How to automatize that?
    // Have a tree of tokens?

    auto nTokens = tokens.size();

    if (nTokens == 0u) {
        if (std::wstring(L"construct").find(lastToken) == 0u)
            possibilities.emplace_back(L"construct");
    }
    else if (nTokens == 1u && tokens[0u] == L"construct") {
        if (std::wstring(L"room").find(lastToken) == 0u)
            possibilities.emplace_back(L"room");
    }
}
