#pragma once

#include <SFML/System/Time.hpp>

#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace context
{
    // Forward declarations

    class Commandable;

    //! A command that will be executed by all objects in category.

    struct Command
    {
        //! The command categories.
        enum class Category
        {
            NONE,
            LOG,    //!< Used for command result.
        };

        Category category = Category::NONE;                         //!< The category of the command.
        std::function<void(Commandable&, const sf::Time&)> action;  //!< Function to execute.
    };

    //! An object able of receiving commands.

    class Commandable
    {
    public:

        //! Constructor.
        Commandable();

        //! Destructor.
        virtual ~Commandable();

        //-----------------//
        //! @name Commands
        //! @{

        //! Set the category.
        virtual Command::Category category() const = 0;

        //! Set the interpreter type.
        virtual std::wstring interpreterType() const { return L""; }

        //! @}

        //---------------//
        //! @name Caster
        //! @{

        //! Downcast to a specific type.
        template<class T> T& as() { return *dynamic_cast<T*>(this); }

        //! @}
    };

    using CommandPtr = std::unique_ptr<Command>;

    //! An object able to interpret a command line.

    class Interpreter
    {
    public:

        //! Constructor.
        Interpreter();

        //! Destructor.
        virtual ~Interpreter();

        //--------------------//
        //! @name Interpreter
        //! @{

        //! Set the interpreter key.
        virtual std::wstring interpreterKey() const = 0;

        //! Interpret a specific command line.
        virtual CommandPtr interpret(const std::vector<std::wstring>& tokens) = 0;

        //! Attempt to autocomplete the command line.
        //! The possibilities to complete the last token will be added at the back of the vector.
        virtual void autoComplete(std::vector<std::wstring>& possibilities,
                                  const std::vector<std::wstring>& tokens, const std::wstring& lastToken) {}

        //! @}
    };
}
