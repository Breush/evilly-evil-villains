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
        void handleMousePressed(const sf::Event& event) override;
        bool handleKeyboardEvent(const sf::Event& event) override;
//        bool handleJoystickEvent(const sf::Event& event) override;

        // Action
        void setText(std::wstring text);
        void setCallback(Callback callback);
        void setAction(std::wstring text, Callback callback);
    };
}

