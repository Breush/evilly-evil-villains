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
            RoomCoords coords{to<uint8>(tokens[2u]), to<uint8>(tokens[3u])};
            m_inter.constructRoom(coords, free);
        }

        // Destroy room
        else if (tokens[0u] == L"destroy" && tokens[1u] == L"room") {
            logMessage = L"> [dungeon] Destroying room " + tokens[2u] + L"/" + tokens[3u];
            bool loss = (nTokens >= 5u) && (tokens[4u] == L"loss");
            RoomCoords coords{to<uint8>(tokens[2u]), to<uint8>(tokens[3u])};
            m_inter.destroyRoom(coords, loss);
        }

        // Remove trap
        else if (tokens[0u] == L"remove" && tokens[1u] == L"room") {
            logMessage = L"> [dungeon] Removing trap from room " + tokens[2u] + L"/" + tokens[3u];
            bool loss = (nTokens >= 5u) && (tokens[4u] == L"loss");
            RoomCoords coords{to<uint8>(tokens[2u]), to<uint8>(tokens[3u])};
            m_inter.removeRoomTrap(coords, loss);
        }

        // Push room
        else if (tokens[0u] == L"pushRoom") {
            logMessage = L"> [dungeon] Pushing room " + tokens[2u] + L"/" + tokens[3u] + L" " + tokens[1u] + L"\n";
            RoomCoords coords{to<uint8>(tokens[2u]), to<uint8>(tokens[3u])};
            auto animationTime = (nTokens >= 5u)? to<uint>(tokens[4u]) : 250u;
            bool success = m_inter.pushRoom(coords, directionFromString(tokens[1u]), animationTime);
            logMessage += L"> [dungeon] --> ";
            logMessage += (success)? L"SUCCESS" : L"FAILURE";
        }
    }

    if (nTokens >= 5u) {
        // Set trap
        if (tokens[0u] == L"set" && tokens[1u] == L"trap") {
            logMessage = L"> [dungeon] Setting trap " + tokens[2u] + L" in room " + tokens[3u] + L"/" + tokens[4u];
            bool free = (nTokens >= 6u) && (tokens[5u] == L"free");
            RoomCoords coords{to<uint8>(tokens[3u]), to<uint8>(tokens[4u])};
            m_inter.setRoomTrap(coords, tokens[2u], free);
        }

        // Create facility
        else if (tokens[0u] == L"create" && tokens[1u] == L"facility") {
            logMessage = L"> [dungeon] Setting trap " + tokens[2u] + L" in room " + tokens[3u] + L"/" + tokens[4u];
            bool free = (nTokens >= 6u) && (tokens[5u] == L"free");
            RoomCoords coords{to<uint8>(tokens[3u]), to<uint8>(tokens[4u])};
            m_inter.createRoomFacility(coords, tokens[2u], free);
        }

        // Remove facility
        else if (tokens[0u] == L"remove" && tokens[1u] == L"facility") {
            logMessage = L"> [dungeon] Removing facility " + tokens[2u] + L" from room " + tokens[3u] + L"/" + tokens[4u];
            bool loss = (nTokens >= 6u) && (tokens[5u] == L"free");
            RoomCoords coords{to<uint8>(tokens[3u]), to<uint8>(tokens[4u])};
            m_inter.removeRoomFacility(coords, tokens[2u], loss);
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
        if (std::wstring(L"construct").find(lastToken) == 0u)       possibilities.emplace_back(L"construct");
        if (std::wstring(L"destroy").find(lastToken) == 0u)         possibilities.emplace_back(L"destroy");
        if (std::wstring(L"set").find(lastToken) == 0u)             possibilities.emplace_back(L"set");
        if (std::wstring(L"create").find(lastToken) == 0u)          possibilities.emplace_back(L"create");
        if (std::wstring(L"remove").find(lastToken) == 0u)          possibilities.emplace_back(L"remove");
        if (std::wstring(L"pushRoom").find(lastToken) == 0u)        possibilities.emplace_back(L"pushRoom");
    }
    else if (nTokens == 1u && tokens[0u] == L"construct") {
        if (std::wstring(L"room").find(lastToken) == 0u)            possibilities.emplace_back(L"room");
    }
    else if (nTokens == 1u && tokens[0u] == L"destroy") {
        if (std::wstring(L"room").find(lastToken) == 0u)            possibilities.emplace_back(L"room");
    }
    else if (nTokens >= 1u && tokens[0u] == L"set") {
        if (nTokens == 1u) {
            if (std::wstring(L"trap").find(lastToken) == 0u)        possibilities.emplace_back(L"trap");
        }
        else if (nTokens == 2u && tokens[1u] == L"trap") {
            for (const auto& trapInfoPair : m_inter.trapsDB().get())
                if (trapInfoPair.first.find(lastToken) == 0u)       possibilities.emplace_back(trapInfoPair.first);
        }
    }
    else if (nTokens >= 1u && tokens[0u] == L"create") {
        if (nTokens == 1u) {
            if (std::wstring(L"facility").find(lastToken) == 0u)    possibilities.emplace_back(L"facility");
        }
        else if (nTokens == 2u && tokens[1u] == L"facility") {
            for (const auto& facilityInfoPair : m_inter.facilitiesDB().get())
                if (facilityInfoPair.first.find(lastToken) == 0u)   possibilities.emplace_back(facilityInfoPair.first);
        }
    }
    else if (nTokens >= 1u && tokens[0u] == L"remove") {
        if (nTokens == 1u) {
            if (std::wstring(L"trap").find(lastToken) == 0u)        possibilities.emplace_back(L"trap");
            if (std::wstring(L"facility").find(lastToken) == 0u)    possibilities.emplace_back(L"facility");
        }
        else if (nTokens == 2u && tokens[1u] == L"facility") {
            for (const auto& facilityInfoPair : m_inter.facilitiesDB().get())
                if (facilityInfoPair.first.find(lastToken) == 0u)   possibilities.emplace_back(facilityInfoPair.first);
        }
    }
    else if (nTokens == 1u && tokens[0u] == L"pushRoom") {
        if (std::wstring(L"north").find(lastToken) == 0u)           possibilities.emplace_back(L"north");
        if (std::wstring(L"south").find(lastToken) == 0u)           possibilities.emplace_back(L"south");
        if (std::wstring(L"east").find(lastToken) == 0u)            possibilities.emplace_back(L"east");
        if (std::wstring(L"west").find(lastToken) == 0u)            possibilities.emplace_back(L"west");
    }
}
