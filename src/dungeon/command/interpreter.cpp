#include "dungeon/command/interpreter.hpp"

#include "dungeon/inter.hpp"
#include "context/logger.hpp"

using namespace dungeon;

Interpreter::Interpreter(Inter* inter)
    : m_inter(*inter)
    , m_roomsInterpreter(m_inter)
    , m_roomInterpreter(m_inter)
{
}

//-----------------------//
//----- Interpreter -----//

// TODO really make commands, used by a commandable inside dungeon
void Interpreter::interpret(std::vector<context::Command>& commands, std::vector<std::wstring>& tokens)
{
    std::wstring logMessage = L"> [dungeon] ";
    auto nTokens = tokens.size();

    if (nTokens >= 1u) {
        if (tokens[0u] == L"rooms") {
            auto newTokens = tokens;
            logMessage += L"Located all dungeon rooms\n";
            m_roomsInterpreter.roomsClear();
            for (uint floor = 0u; floor < m_inter.data().floorsCount(); ++floor)
            for (uint floorRoom = 0u; floorRoom < m_inter.data().floorRoomsCount(); ++floorRoom) {
                RoomCoords coords{static_cast<uint8>(floor), static_cast<uint8>(floorRoom)};
                m_roomsInterpreter.roomsAdd(coords);
            }

            newTokens.erase(std::begin(newTokens));
            m_roomsInterpreter.interpret(commands, newTokens);
            goto logging;
        }
    }

    if (nTokens >= 3u) {
        if (tokens[0u] == L"room") {
            auto newTokens = tokens;
            logMessage += L"Located room " + tokens[1u] + L"/" + tokens[2u] + L"\n";
            RoomCoords coords{to<uint8>(tokens[1u]), to<uint8>(tokens[2u])};

            m_roomInterpreter.roomSet(coords);
            newTokens.erase(std::begin(newTokens), std::begin(newTokens) + 3u);
            m_roomInterpreter.interpret(commands, newTokens);
            goto logging;
        }
    }

    if (nTokens >= 4u) {
        if (tokens[0u] == L"generic") {
            if (tokens[1u] == L"monsters") {
                if (tokens[2u] == L"unlock") {
                    // TODO Use "free" parameter -> let m_inter manage the cost, and therefore update the Hub Market/Inn states
                    logMessage += L"Unlocking generic monster " + tokens[3u];
                    bool all = (tokens[3u] == L"*");
                    if (all)    m_inter.data().setMonstersGenericUnlocked(true);
                    else        m_inter.data().setMonsterGenericUnlocked(tokens[3u], true);
                    goto logging;
                }
            }
            else if (tokens[1u] == L"traps") {
                if (tokens[2u] == L"unlock") {
                    logMessage += L"Unlocking generic trap " + tokens[3u];
                    bool all = (tokens[3u] == L"*");
                    if (all)    m_inter.data().setTrapsGenericUnlocked(true);
                    else        m_inter.data().setTrapGenericUnlocked(tokens[3u], true);
                    goto logging;
                }
            }
        }
        else if (tokens[0u] == L"structure") {
            // Floors count
            if (tokens[1u] == L"floorsCount") {
                if (tokens[2u] == L"add") {
                    logMessage += L"Adding " + tokens[3u] + L" floors to floors count";
                    m_inter.addFloorsCount(to<int>(tokens[3u]));
                    goto logging;
                }
                else if (tokens[2u] == L"set") {
                    logMessage += L"Setting floors count to " + tokens[3u] + L" floors";
                    m_inter.setFloorsCount(to<uint>(tokens[3u]));
                    goto logging;
                }
            }

            // Floor rooms count
            else if (tokens[1u] == L"floorRoomsCount") {
                if (tokens[2u] == L"add") {
                    logMessage += L"Adding " + tokens[3u] + L" rooms to floor rooms count";
                    m_inter.addFloorRoomsCount(to<int>(tokens[3u]));
                    goto logging;
                }
                else if (tokens[2u] == L"set") {
                    logMessage += L"Setting floor rooms count to " + tokens[3u] + L" rooms";
                    m_inter.setFloorRoomsCount(to<uint>(tokens[3u]));
                    goto logging;
                }
            }
        }
    }

    return;

    logging:
    context::addCommandLog(commands, logMessage);
}

void Interpreter::autoComplete(std::vector<std::wstring>& possibilities,
                               const std::vector<std::wstring>& tokens, const std::wstring& lastToken)
{
    auto nTokens = tokens.size();
    auto checkAdd = [&possibilities, &lastToken] (const std::wstring& string) {
        if (string.find(lastToken) == 0u)
            possibilities.emplace_back(string);
    };

    if (nTokens == 0u) {
        checkAdd(L"generic");
        checkAdd(L"room");
        checkAdd(L"rooms");
        checkAdd(L"structure");
        return;
    }
    else if (nTokens == 1u) {
        if (tokens[0u] == L"generic") {
            checkAdd(L"monsters");
            checkAdd(L"traps");
            return;
        }
        else if (tokens[0u] == L"structure") {
            checkAdd(L"floorsCount");
            checkAdd(L"floorRoomsCount");
            return;
        }
    }

    else if (nTokens == 2u) {
        if (tokens[0u] == L"generic") {
            if (tokens[1u] == L"monsters" || tokens[1u] == L"traps") {
                checkAdd(L"unlock");
                return;
            }
        }
        else if (tokens[0u] == L"structure") {
            if (tokens[1u] == L"floorsCount" || tokens[1u] == L"floorRoomsCount") {
                checkAdd(L"add");
                checkAdd(L"set");
                return;
            }
        }
    }
    else if (nTokens == 3u) {
        if (tokens[0u] == L"generic") {
            if (tokens[1u] == L"monsters") {
                if (tokens[2u] == L"unlock") {
                    checkAdd(L"*");
                    for (const auto& monsterInfoPair : m_inter.monstersDB().get())
                        checkAdd(monsterInfoPair.first);
                    return;
                }
            }
            else if (tokens[1u] == L"traps") {
                if (tokens[2u] == L"unlock") {
                    checkAdd(L"*");
                    for (const auto& trapInfoPair : m_inter.trapsDB().get())
                        checkAdd(trapInfoPair.first);
                    return;
                }
            }
        }
    }

    if (nTokens >= 1u) {
        if (tokens[0u] == L"rooms") {
            auto newTokens = tokens;
            newTokens.erase(std::begin(newTokens));
            m_roomsInterpreter.autoComplete(newTokens, checkAdd);
            return;
        }
    }

    if (nTokens >= 3u) {
        if (tokens[0u] == L"room") {
            auto newTokens = tokens;
            newTokens.erase(std::begin(newTokens), std::begin(newTokens) + 3u);
            m_roomInterpreter.autoComplete(newTokens, checkAdd);
            return;
        }
    }
}
