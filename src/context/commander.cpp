#include "context/commander.hpp"

#include "context/logger.hpp"
#include "tools/debug.hpp"
#include "tools/platform-fixes.hpp" // erase_if

using namespace context;

//-------------------//
//----- Routine -----//

void Commander::update(const sf::Time& dt)
{
    // For each command, test each commandable
    while (!m_commandQueue.empty()) {
        // TODO OPTIM Is that doing a useless copy?
        auto command = pop();
        for (auto commandable : m_commandables)
            if (commandable->category() == command.category)
                command.action(*commandable, dt);
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

Command Commander::interpret(const std::wstring& commandLine)
{
    // TODO Interpret!

    // Unknown command
    Command command;
    command.category = Command::Category::LOG;
    command.action = [commandLine] (Commandable& commandable, const sf::Time&)
        { commandable.as<Logger>().commandLog(L"> Unknown command: " + commandLine); };
    return command;
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
