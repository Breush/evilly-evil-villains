#include "context/commander.hpp"

#include "context/logger.hpp"
#include "tools/platform-fixes.hpp"
#include "tools/string.hpp"
#include "tools/debug.hpp"
#include "tools/tools.hpp"
#include "tools/time.hpp"

using namespace context;

Commander::Commander()
{
    auto logFileName = "log/commands_" + time2string("%Y%m%d-%H%M%S") + ".eev";
    m_logStream.open(logFileName);
}

Commander::~Commander()
{
    if (m_logStream.is_open())
        m_logStream.close();
}

//-------------------//
//----- Routine -----//

void Commander::update(const sf::Time& dt)
{
    while (!m_commandQueue.empty()) {
        const auto& command = m_commandQueue.front();

        // Log the line if it is written
        if (!command.line.empty())
            m_logStream << command.line << std::endl;

        // Test each commandable
        for (auto pCommandable : m_commandables)
            if (pCommandable->category() == command.category)
                command.action(*pCommandable, dt);

        m_commandQueue.pop();
    }
}

//-----------------//
//----- Queue -----//

void Commander::push(const Command& command)
{
    m_commandQueue.push(command);
}

Command Commander::pop()
{
    auto command = m_commandQueue.front();
    m_commandQueue.pop();
    return command;
}

//-----------------------//
//----- Interpreter -----//

// TODO We should return a vector of commands, right?
CommandPtr Commander::interpret(const std::wstring& commandLine)
{
    returnif (commandLine.empty()) nullptr;

    // Get tokens
    auto tokens = split(commandLine);
    auto key = tokens.front();
    tokens.erase(std::begin(tokens));

    // Help - list possibilities
    if (key == L"help") {
        std::vector<std::wstring> keys;
        for (const auto& interpreter : m_interpreters)
            keys.emplace_back(interpreter->interpreterKey());

        auto pCommand = std::make_unique<Command>();
        setCommandLog(*pCommand, L"> Available command keys: " + join(keys, std::wstring(L", ")));
        return std::move(pCommand);
    }

    // Forward to interpreters
    for (auto pInterpreter : m_interpreters) {
        if (pInterpreter->interpreterKey() == key) {
            auto pCommand = pInterpreter->interpret(tokens);
            if (pCommand != nullptr)
                pCommand->line = key + L" " + join(tokens, std::wstring(L" "));
            return std::move(pCommand);
        }
    }

    // Unknown command
    return nullptr;
}

std::wstring Commander::autoComplete(std::wstring commandLine)
{
    // Get tokens
    bool isTokenStart = commandLine.empty() || iswblank(commandLine.back());
    auto tokens = split(commandLine);

    std::wstring lastToken;
    if (!isTokenStart) {
        lastToken = tokens.back();
        tokens.erase(std::end(tokens));
    }

    // Check if it is the key to autocomplete
    std::vector<std::wstring> possibilities;
    if (tokens.empty()) {
        // Find all interpreter keys starting with the last token
        for (auto pInterpreter : m_interpreters) {
            auto key = pInterpreter->interpreterKey();
            if (key.find(lastToken) == 0u)
                possibilities.emplace_back(std::move(key));
        }
    }

    // Else, broadcast auto-complete to interpreters
    else {
        auto key = tokens.front();
        tokens.erase(std::begin(tokens));

        for (auto pInterpreter : m_interpreters)
            if (pInterpreter->interpreterKey() == key)
                pInterpreter->autoComplete(possibilities, tokens, lastToken);
    }

    // No possibility, keep previous
    returnif (possibilities.size() == 0u) commandLine;

    std::wstring newToken = possibilities.front() + L' ';

    // If more than one possibilities, log them
    if (possibilities.size() > 1u) {
        // Find common denominator and construct list
        for (const auto& possibility : possibilities)
            newToken = prefix(newToken, possibility);

        // Log them
        Command command;
        push(setCommandLog(command, join(possibilities)));
    }

    // No new token to set
    returnif (newToken.empty()) commandLine;

    // Affect new token
    commandLine = commandLine.substr(0u, commandLine.size() - lastToken.size());
    commandLine += newToken;
    return commandLine;
}

//--------------------//
//----- Register -----//

void Commander::add(Commandable* pCommandable)
{
    m_commandables.emplace_back(pCommandable);
}

void Commander::remove(Commandable* pCommandable)
{
    std::erase_if(m_commandables, [pCommandable] (Commandable* pElement) { return pElement == pCommandable; } );
}

void Commander::add(Interpreter* pInterpreter)
{
    m_interpreters.emplace_back(pInterpreter);
}

void Commander::remove(Interpreter* pInterpreter)
{
    std::erase_if(m_interpreters, [pInterpreter] (Interpreter* pElement) { return pElement == pInterpreter; } );
}
