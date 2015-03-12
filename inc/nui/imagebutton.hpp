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
        void setVisual(const std::wstring& text, TextureID imageID, const sf::Vector2f& inImageSize);

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! The size of the image.
        PARAMGSU(sf::Vector2f, m_imageSize, imageSize, setImageSize, updateSize);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void update() final;

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
    };
}
