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

context::CommandPtr Commandable::interpret(const std::vector<std::wstring>& tokens)
{
    std::wstring logMessage;
    const std::wstring logStart(L"> [dungeon] ");
    auto nTokens = tokens.size();

    if (nTokens >= 3u) {
        // Floors count
        if (tokens[0u] == L"floorsCount") {
            if (tokens[1u] == L"add") {
                logMessage = logStart + L"Adding " + tokens[2u] + L" floors to floors count";
                m_inter.addFloorsCount(to<int>(tokens[2u]));
            }
            else if (tokens[1u] == L"set") {
                logMessage = logStart + L"Setting floors count to " + tokens[2u] + L" floors";
                m_inter.setFloorsCount(to<uint>(tokens[2u]));
            }
        }

        // Floor rooms count
        if (tokens[0u] == L"floorRoomsCount") {
            if (tokens[1u] == L"add") {
                logMessage = logStart + L"Adding " + tokens[2u] + L" rooms to floor rooms count";
                m_inter.addFloorRoomsCount(to<int>(tokens[2u]));
            }
            else if (tokens[1u] == L"set") {
                logMessage = logStart + L"Setting floor rooms count to " + tokens[2u] + L" rooms";
                m_inter.setFloorRoomsCount(to<uint>(tokens[2u]));
            }
        }
    }

    if (nTokens >= 4u) {
        if (tokens[0u] == L"room") {
            // Room control
            if (tokens[3u] == L"construct") {
                logMessage = logStart + L"Constructing room " + tokens[1u] + L"/" + tokens[2u];
                RoomCoords coords{to<uint8>(tokens[1u]), to<uint8>(tokens[2u])};
                bool free = (nTokens >= 5u) && (tokens[4u] == L"free");
                m_inter.constructRoom(coords, free);
            }
            else if (tokens[3u] == L"destroy") {
                logMessage = logStart + L"Destroying room " + tokens[1u] + L"/" + tokens[2u];
                RoomCoords coords{to<uint8>(tokens[1u]), to<uint8>(tokens[2u])};
                bool loss = (nTokens >= 5u) && (tokens[4u] == L"loss");
                m_inter.destroyRoom(coords, loss);
            }
        }
    }

    if (nTokens >= 5u) {
        if (tokens[0u] == L"room") {
            // Room control
            if (tokens[3u] == L"push") {
                logMessage = logStart + L"Pushing room " + tokens[1u] + L"/" + tokens[2u] + L" " + tokens[4u] + L"\n";
                RoomCoords coords{to<uint8>(tokens[1u]), to<uint8>(tokens[2u])};
                auto direction = directionFromString(tokens[4u]);
                auto animationTime = (nTokens >= 6u)? to<uint>(tokens[5u]) : 250u;
                bool success = m_inter.pushRoom(coords, direction, animationTime);
                logMessage += logStart + L"--> " + ((success)? L"SUCCESS" : L"FAILURE");
            }

            // Trap
            else if (tokens[3u] == L"trap") {
                if (tokens[4u] == L"remove") {
                    logMessage = logStart + L"Removing trap from room " + tokens[1u] + L"/" + tokens[2u];
                    RoomCoords coords{to<uint8>(tokens[1u]), to<uint8>(tokens[2u])};
                    bool loss = (nTokens >= 6u) && (tokens[5u] == L"loss");
                    m_inter.removeRoomTrap(coords, loss);
                }
            }
        }
    }

    if (nTokens >= 6u) {
        if (tokens[0u] == L"room") {
            // Trap
            if (tokens[3u] == L"trap") {
                if (tokens[4u] == L"remove") {
                    logMessage = logStart + L"Setting trap " + tokens[5u] + L" in room " + tokens[1u] + L"/" + tokens[2u];
                    RoomCoords coords{to<uint8>(tokens[1u]), to<uint8>(tokens[2u])};
                    bool free = (nTokens >= 7u) && (tokens[6u] == L"free");
                    m_inter.setRoomTrap(coords, tokens[5u], free);
                }
            }

            // Facility
            else if (tokens[3u] == L"facility") {
                if (tokens[4u] == L"create") {
                    logMessage = logStart + L"Setting trap " + tokens[2u] + L" in room " + tokens[3u] + L"/" + tokens[4u];
                    RoomCoords coords{to<uint8>(tokens[1u]), to<uint8>(tokens[2u])};
                    bool free = (nTokens >= 7u) && (tokens[6u] == L"free");
                    m_inter.createRoomFacility(coords, tokens[5u], free);
                }
                else if (tokens[4u] == L"remove") {
                    logMessage = logStart + L"Removing facility " + tokens[5u] + L" from room " + tokens[1u] + L"/" + tokens[2u];
                    RoomCoords coords{to<uint8>(tokens[1u]), to<uint8>(tokens[2u])};
                    bool loss = (nTokens >= 7u) && (tokens[6u] == L"loss");
                    m_inter.removeRoomFacility(coords, tokens[5u], loss);
                }
            }
        }
    }

    returnif (logMessage.empty()) nullptr;

    // Generate log
    auto pCommand = std::make_unique<context::Command>();
    context::setCommandLog(*pCommand, logMessage);
    return std::move(pCommand);
}

void Commandable::autoComplete(std::vector<std::wstring>& possibilities,
                               const std::vector<std::wstring>& tokens, const std::wstring& lastToken)
{
    auto nTokens = tokens.size();
    auto checkAdd = [&possibilities, &lastToken] (const std::wstring& string) {
        if (string.find(lastToken) == 0u)
            possibilities.emplace_back(string);
    };

    if (nTokens == 0u) {
        checkAdd(L"floorsCount");
        checkAdd(L"floorRoomsCount");
        checkAdd(L"room");
    }
    else if (nTokens == 1u) {
        if (tokens[0u] == L"floorsCount" || tokens[0u] == L"floorRoomsCount") {
            checkAdd(L"add");
            checkAdd(L"set");
        }
    }
    else if (nTokens == 3u) {
        if (tokens[0u] == L"room") {
            checkAdd(L"construct");
            checkAdd(L"destroy");
            checkAdd(L"push");
            checkAdd(L"trap");
            checkAdd(L"facility");
        }
    }
    else if (nTokens == 4u) {
        if (tokens[0u] == L"room") {
            if (tokens[3u] == L"construct") {
                checkAdd(L"free");
            }
            else if (tokens[3u] == L"destroy") {
                checkAdd(L"loss");
            }
            else if (tokens[3u] == L"push") {
                checkAdd(L"north");
                checkAdd(L"south");
                checkAdd(L"west");
                checkAdd(L"east");
            }
            else if (tokens[3u] == L"trap") {
                checkAdd(L"set");
                checkAdd(L"remove");
            }
            else if (tokens[3u] == L"facility") {
                checkAdd(L"create");
                checkAdd(L"remove");
            }
        }
    }
    else if (nTokens == 5u) {
        if (tokens[0u] == L"room") {
            if (tokens[3u] == L"trap") {
                if (tokens[4u] == L"set") {
                    for (const auto& trapInfoPair : m_inter.trapsDB().get())
                        checkAdd(trapInfoPair.first);
                }
                if (tokens[4u] == L"remove") {
                    checkAdd(L"loss");
                }
            }
            else if (tokens[3u] == L"facility") {
                if (tokens[4u] == L"create" || tokens[4u] == L"remove") {
                    for (const auto& facilityInfoPair : m_inter.facilitiesDB().get())
                        checkAdd(facilityInfoPair.first);
                }
            }
        }
    }
    else if (nTokens == 6u) {
        if (tokens[0u] == L"room") {
            if (tokens[3u] == L"trap") {
                if (tokens[4u] == L"set") {
                    checkAdd(L"free");
                }
            }
            if (tokens[3u] == L"facility") {
                if (tokens[4u] == L"create") {
                    checkAdd(L"free");
                }
                if (tokens[4u] == L"remove") {
                    checkAdd(L"loss");
                }
            }
        }
    }
}
