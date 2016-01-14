#pragma once

#include "nui/entity.hpp"

#include <functional>

namespace nui
{
    //! A pushable button to execute actions.
    /*!
     *  This button shows a specific text and executes action on click.
     */

    class PushButton : public nui::Entity
    {
        using baseClass = nui::Entity;

        //! The type of function callback when the button is pressed.
        using ValidateCallback = std::function<void()>;

    public:

        //! Constructor.
        PushButton();

        //! Default destructor.
        virtual ~PushButton() = default;

        virtual std::string _name() const override { return "nui::PushButton"; }

        //----------------//
        //! @name Actions
        //! @{

        //! Set the displayed text.
        void setText(const std::wstring& text);

        //! Set the displayed image.
        void setImage(const std::string& textureID);

        //! Set the action to do when button is pressed.
        void setValidateCallback(const ValidateCallback validateCallback);

        //! Set the action to do when button is pressed.
        void set(const std::wstring& text, const std::string& imageTextureID);

        //! Set the action and the text.
        void set(const std::wstring& text, const ValidateCallback validateCallback);

        //! @}

        //------------------//
        //! @name Callbacks
        //! @{

        //! Execute the callback if any.
        void validate();

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() override;
        void refreshNUI(const config::NUIGuides& cNUI) override;

        //! Update the absolute size.
        void updateSize();

        //! Update the image texture.
        void updateImageTexture();

        //! @}


        //---------------//
        //! @name Events
        //! @{

        inline bool handleGlobalEvents() const final { return true; }
        void handleGlobalMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f& nuiPos) final;
        bool handleKeyboardEvent(const sf::Event& event) final;
        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Refresh text and decorum positions.
        void refresh();

        //! @}

    private:

        // Text
        sf::Text m_text;    //!< The displayed text.

        // Image
        sf::RectangleShape m_image;     //!< The displayed image.
        std::string m_imageTextureID;   //!< The displayed image textureID (empty if none).
        bool m_imageActive = false;     //!< Is there an image currently displayed?

        // Sprites
        sf::RectangleShape m_left;      //!< Left part of the button.
        sf::RectangleShape m_middle;    //!< Middle part of the button.
        sf::RectangleShape m_right;     //!< Right part of the button.

        // Callback
        ValidateCallback m_validateCallback = nullptr;  //!< The action to execute when clicked.

        // States
        bool m_pressed = false; //!< Is the button currently pressed?
        bool m_hovered = false; //!< Is the button currently hovered?

        // Decorum
        float m_fontSize = 0.f;         //!< Font size as defined in the NUI guides.
        float m_vPadding = 0.f;         //!< Vertical padding as defined in the NUI guides.
        float m_hPadding = 0.f;         //!< Horizontal padding as defined in the NUI guides.
        float m_imageHintSize = 0.f;    //!< Forced image width.
    };
}

