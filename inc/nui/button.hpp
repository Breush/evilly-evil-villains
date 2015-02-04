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

        // Virtual
        virtual void init() override;

        // Get events to valid action
        void handleMousePressed(const sf::Event& event, const sf::Vector2f& absPos, const sf::Vector2f& relPos) override;
        bool handleKeyboardEvent(const sf::Event& event) override;
//        bool handleJoystickEvent(const sf::Event& event) override;

        // Action
        void setText(const std::wstring& text);
        void setCallback(const Callback callback);
        void setAction(const std::wstring& text, const Callback callback);
    };
}

