#pragma once

#include "nui/choicebox.hpp"

namespace nui
{
    //! A button to execute actions.
    /*!
     *  This button shows a specific text and executes action on click.
     *  It derives from nui::ChoiceBox for its display style.
     */

    class Button : public ChoiceBox
    {
        using baseClass = ChoiceBox;

    public:

        //! Constructor.
        Button();

        //! Default destructor.
        virtual ~Button() = default;

        //----------------//
        //! @name Actions
        //! @{

        //! Wrapper to call nui::ChoiceBox::setChoiceText().
        void setText(const std::wstring& text);

        //! Wrapper to call nui::ChoiceBox::setChoiceCallback().
        void setCallback(const Callback callback);

        //! Calls both setText() and setCallback() functions.
        void setAction(const std::wstring& text, const Callback callback);

        //! @}

    protected:

        //---------------//
        //! @name Events
        //! @{

        bool handleKeyboardEvent(const sf::Event& event) final;
        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;

        //! @}
    };
}

