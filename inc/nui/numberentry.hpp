#pragma once

#include "nui/textentry.hpp"

#include <sstream>

namespace nui
{
    //! An interactive entry allowing user to type numbers.

    template<typename Value_t>
    class NumberEntry final : public nui::TextEntry
    {
        using baseClass = nui::TextEntry;

        //! Callback type used when a change occurs.
        using ValueChangeCallback = std::function<void(Value_t, Value_t)>;

    public:

        //! Default constructor.
        NumberEntry() = default;

        //! Default destructor.
        ~NumberEntry() = default;

        std::string _name() const final { return "nui::NumberEntry"; }

        //---------------//
        //! @name Value
        //! @{

        //! Get the value.
        Value_t value() const { Value_t outValue; std::wstringstream str(text()); str >> outValue; return outValue; }

        //! Sets the value, if sendCallback is false a callback will not be sent.
        void setValue(Value_t value, bool sendCallback = true);

        //! @}

        //-----------------//
        //! @name Callback
        //! @{

        //! Sets the callback called whenever the value changes.
        void setOnValueChangeCallback(const ValueChangeCallback& callback);

        //! @}

    protected:

        //---------------//
        //! @name Events
        //! @{

        bool handleKeyboardEvent(const sf::Event& event) final;

        //! @}

    private:

        //! Whenever the value changes.
        ValueChangeCallback m_onValueChangeCallback;
    };
}

#include "nui/numberentry.inl"
