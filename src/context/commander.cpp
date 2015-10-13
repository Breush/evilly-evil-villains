#include "context/commander.hpp"

#include "context/logger.hpp"
#include "tools/platform-fixes.hpp" // erase_if
#include "tools/string.hpp"
#include "tools/debug.hpp"
#include "tools/tools.hpp"

using namespace context;

//-------------------//
//----- Routine -----//

void Commander::update(const sf::Time& dt)
{
    // For each command, test each commandable
    while (!m_commandQueue.empty()) {
        const auto& command = m_commandQueue.front();
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
Command Commander::interpret(const std::wstring& commandLine)
{
    returnif (commandLine.empty()) Command();

    // Get tokens
    auto tokens = split(commandLine);
    auto key = tokens.front();
    tokens.erase(std::begin(tokens));

    // Help - list possibilities
    if (key == L"help") {
        std::wstring knownKeys;
        for (auto pInterpreter : m_interpreters)
            knownKeys += pInterpreter->interpreterKey() + L", ";

        Command command;
        command.category = Command::Category::LOG;
        command.action = [knownKeys] (Commandable& commandable, const sf::Time&)
            { commandable.as<Logger>().commandLog(L"> Available command keys: " + knownKeys); };
        return command;
    }

    // Forward to interpreters
    for (auto pInterpreter : m_interpreters)
        if (pInterpreter->interpreterKey() == key)
            return pInterpreter->interpret(tokens);

    // Unknown command
    Command command;
    return setCommandLog(command, L"> Unknown command key: " + commandLine);
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
