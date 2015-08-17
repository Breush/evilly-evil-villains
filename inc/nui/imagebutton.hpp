#pragma once

#include "nui/button.hpp"

#include <SFML/Graphics/Sprite.hpp>

// Forward declarations

enum class TextureID : uint8;

namespace nui
{
    //! A button with an image to execute actions.
    /*!
     *  This button shows an image and a specific text.
     */

    class ImageButton : public Button
    {
        using baseClass = Button;

    public:

        //! Constructor.
        ImageButton();

        //! Default destructor.
        virtual ~ImageButton() = default;

        //----------------//
        //! @name Visuals
        //! @{

        //! Set the image to be displayed.
        void setImage(TextureID imageID);

        //! Set all the visual information.
        void setVisual(const std::wstring& text, TextureID imageID);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        void handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;

        //! @}

        //-------------------------------//
        //! @name Internal state updates
        //! @{

        void updateSize() final;

        //! @}

    private:

        //! The image to be displayed.
        sf::Sprite m_image;

        sf::Vector2f m_imageSize;   //!< The reference size of the image.
        float m_vPadding = 0.f;     //!< The space between the image and the text.
    };
}
