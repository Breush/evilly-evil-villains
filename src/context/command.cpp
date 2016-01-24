#include "context/command.hpp"

#include "context/context.hpp"

using namespace context;

//-----------------------//
//----- Commandable -----//

Commandable::Commandable()
{
    context::context.commander.add(this);
}

Commandable::~Commandable()
{
    context::context.commander.remove(this);
}

//-----------------------//
//----- Interpreter -----//

Interpreter::Interpreter()
{
    context::context.commander.add(this);
}

Interpreter::~Interpreter()
{
    context::context.commander.remove(this);
}
