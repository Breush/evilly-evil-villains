#pragma once

#include <SFML/Window/Event.hpp>

namespace nui
{
    //------------------//
    //----- Events -----//

    template<typename Value_t>
    bool NumberEntry<Value_t>::handleKeyboardEvent(const sf::Event& event)
    {
        if (event.type == sf::Event::TextEntered) {
            if (event.text.unicode != 13        // Return
                && event.text.unicode != 8      // Backspace
                && event.text.unicode != 127    // Delete
                && (event.text.unicode < 48     // 0
                || event.text.unicode > 57))    // 9
                return false;
        }

        return baseClass::handleKeyboardEvent(event);
    }

    //-----------------//
    //----- Value -----//

    template<typename Value_t>
    void NumberEntry<Value_t>::setValue(Value_t value, bool sendCallback)
    {
        std::wstringstream str;
        str << value;
        setText(str.str(), sendCallback);
    }

    //--------------------//
    //----- Callback -----//

    template<typename Value_t>
    void NumberEntry<Value_t>::setOnValueChangeCallback(const ValueChangeCallback& callback)
    {
        m_onValueChangeCallback = callback;

        if (m_onValueChangeCallback == nullptr) {
            setOnTextChangeCallback(nullptr);
            return;
        }

        setOnTextChangeCallback([this](const std::wstring& oldText, const std::wstring& text) {
            Value_t oldValue, value;
            std::wstringstream str(oldText); str >> oldValue;
            str.clear(); str.str(text); str >> value;
            m_onValueChangeCallback(oldValue, value);
        });
    }
}
