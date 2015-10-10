#pragma once

#include <SFML/System/Time.hpp>

#include <functional>

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

        //! @}

        //---------------//
        //! @name Caster
        //! @{

        //! Downcast to a specific type.
        template<class T> T& as() { return *dynamic_cast<T*>(this); }

        //! @}
    };
}
