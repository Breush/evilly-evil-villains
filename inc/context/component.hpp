#pragma once

namespace context
{
    //! Abstract class designed to be the interface of all scene components.

    class Component
    {
    public:

        //! Default constuctor.
        Component() = default;

        //! Default destructor.
        virtual ~Component() = default;
    };
}
