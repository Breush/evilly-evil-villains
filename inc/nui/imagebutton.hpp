#pragma once

#include "nui/button.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace nui
{
    class ImageButton : public Button
    {
        typedef Button baseClass;

    public:
        ImageButton();
        virtual ~ImageButton() {}

        // Action
        void setImage(Textures::ID imageID);
        void setVisual(const std::wstring& text, Textures::ID imageID, const sf::Vector2f& inImageSize);

    protected:
        // Virtual
        virtual void update() override;

        // Mouse events
        virtual void handleMouseMoved(const sf::Vector2f& mousePos) override;

        // Updates
        virtual void updateSize() override;

        // Params
        PARAMGSU(sf::Vector2f, m_imageSize, imageSize, setImageSize, updateSize);

    private:
        sf::Sprite m_image;
    };
}
