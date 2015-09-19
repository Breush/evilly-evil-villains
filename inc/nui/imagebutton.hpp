#pragma once

#include "nui/button.hpp"

#include <SFML/Graphics/Sprite.hpp>

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
        void setImage(const std::string& imageID);

        //! Set all the visual information.
        void setVisual(const std::wstring& text, const std::string& imageID);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        void handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        void updateSize() final;

        //! Refresh image/text positions.
        void refreshElementsPositions();

        //! @}

    private:

        //! The image to be displayed.
        sf::Sprite m_image;

        sf::Vector2f m_imageSize;   //!< The reference size of the image.
        float m_vPadding = 0.f;     //!< The space between the image and the text.
    };
}
