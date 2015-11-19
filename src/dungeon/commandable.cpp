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

        // Destroy room
        else if (tokens[0u] == L"destroy" && tokens[1u] == L"room") {
            logMessage = L"> [dungeon] Destroying room " + tokens[2u] + L"/" + tokens[3u];
            bool free = (nTokens >= 5u) && (tokens[4u] == L"loss");
            m_inter.destroyRoom({to<uint>(tokens[2u]), to<uint>(tokens[3u])}, free);
        }

        // Remove trap
        else if (tokens[0u] == L"remove" && tokens[1u] == L"room") {
            logMessage = L"> [dungeon] Removing trap in room " + tokens[2u] + L"/" + tokens[3u];
            bool free = (nTokens >= 5u) && (tokens[4u] == L"loss");
            m_inter.removeRoomTrap({to<uint>(tokens[2u]), to<uint>(tokens[3u])}, free);
        }
    }

    if (nTokens >= 5u) {
        // Set trap
        if (tokens[0u] == L"set" && tokens[1u] == L"trap") {
            logMessage = L"> [dungeon] Setting trap " + tokens[2u] + L" in room " + tokens[3u] + L"/" + tokens[4u];
            bool free = (nTokens >= 6u) && (tokens[5u] == L"free");
            m_inter.setRoomTrap({to<uint>(tokens[3u]), to<uint>(tokens[4u])}, tokens[2u], free);
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
        if (std::wstring(L"construct").find(lastToken) == 0u)   possibilities.emplace_back(L"construct");
        if (std::wstring(L"destroy").find(lastToken) == 0u)     possibilities.emplace_back(L"destroy");
        if (std::wstring(L"set").find(lastToken) == 0u)         possibilities.emplace_back(L"set");
        if (std::wstring(L"remove").find(lastToken) == 0u)      possibilities.emplace_back(L"remove");
    }
    else if (nTokens == 1u && tokens[0u] == L"construct") {
        if (std::wstring(L"room").find(lastToken) == 0u)        possibilities.emplace_back(L"room");
    }
    else if (nTokens == 1u && tokens[0u] == L"destroy") {
        if (std::wstring(L"room").find(lastToken) == 0u)        possibilities.emplace_back(L"room");
    }
    else if (nTokens >= 1u && tokens[0u] == L"set") {
        if (nTokens == 1u) {
            if (std::wstring(L"trap").find(lastToken) == 0u)    possibilities.emplace_back(L"trap");
        }
        else if (nTokens == 2u && tokens[1u] == L"trap") {
            for (const auto& trapInfoPair : m_inter.trapsDB().get())
                if (trapInfoPair.first.find(lastToken) == 0u)   possibilities.emplace_back(trapInfoPair.first);
        }
    }
    else if (nTokens == 1u && tokens[0u] == L"remove") {
        if (std::wstring(L"trap").find(lastToken) == 0u)        possibilities.emplace_back(L"trap");
    }
}
