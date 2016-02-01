#pragma once

#include "context/command.hpp"

namespace dungeon
{
    // Forward declarations

    class Inter;

    //! The interpreter of the dungeon.

    class Commandable final : private context::Interpreter
    {
        using baseClass = Commandable;

        std::wstring interpreterKey() const final { return L"dungeon"; }

    public:

        //! Constructor.
        Commandable(Inter* inter);

        //! Default destructor.
        ~Commandable() = default;

    protected:

        //--------------------//
        //! @name Interpreter
        //! @{

        context::CommandPtr interpret(const std::vector<std::wstring>& tokens) final;
        void autoComplete(std::vector<std::wstring>& possibilities,
                          const std::vector<std::wstring>& tokens, const std::wstring& lastToken) final;

        //! @}

    private:

        Inter& m_inter;     //!< Reference to the inter;
    };
}
