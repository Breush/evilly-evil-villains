#pragma once

#include "nui/entity.hpp"

namespace nui
{
    //! A simple display that shows options for mouse.

    class MouseOverlay final : public nui::Entity
    {
    public:

        //! Constructor.
        MouseOverlay();

        //! Default destructor.
        ~MouseOverlay() = default;

        std::string _name() const final { return "nui::MouseOverlay"; }

        //----------------//
        //! @name Control
        //! @{

        //! Set the left action. Set empty string to disable it.
        void setLeft(const std::wstring& actionName);

        //! Set the right action. Set empty string to disable it.
        void setRight(const std::wstring& actionName);

        //! @}

    protected:

        //------------//
        //! @name ICU
        //! @{

        //! Select the correct texture for the mouse given the current texts.
        void refreshMouseTexture();

        //! @}

    private:

        sf::Text m_leftText;        //!< The left text.
        sf::Text m_rightText;       //!< The right text.
        sf::RectangleShape m_mouse; //!< The mouse drawn.
    };
}
