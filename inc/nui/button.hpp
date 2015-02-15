#pragma once

#include "nui/choicebox.hpp"

namespace nui
{
    class Button : public ChoiceBox
    {
        typedef ChoiceBox baseClass;

    public:
        Button();
        virtual ~Button() {}

        // Action
        void setText(const std::wstring& text);
        void setCallback(const Callback callback);
        void setAction(const std::wstring& text, const Callback callback);

    protected:
        // Events
        virtual void handleMouseButtonPressed(const sf::Mouse::Button& button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        virtual void handleKeyboardEvent(const sf::Event& event) override;
    };
}

