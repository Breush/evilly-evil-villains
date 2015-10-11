#include "context/command.hpp"

#include "core/application.hpp"

using namespace context;

//-----------------------//
//----- Commandable -----//

Commandable::Commandable()
{
    Application::context().commander.add(this);
}

Commandable::~Commandable()
{
    Application::context().commander.remove(this);
}

//-----------------------//
//----- Interpreter -----//

Interpreter::Interpreter()
{
    Application::context().commander.add(this);
}

Interpreter::~Interpreter()
{
    Application::context().commander.remove(this);
}
