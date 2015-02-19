#pragma once

#include "nui/button.hpp"

#include <SFML/Graphics/Sprite.hpp>

// Forward declarations

enum class TextureID : uint8;

namespace nui
{
    class ImageButton : public Button
    {
        using baseClass = Button;

    public:
        ImageButton();
        virtual ~ImageButton() {}
        ImageButton(const ImageButton& copy);

        // Action
        void setImage(TextureID imageID);
        void setVisual(const std::wstring& text, TextureID imageID, const sf::Vector2f& inImageSize);

    protected:
        // Virtual
        virtual void update() override;

        // Mouse events
        virtual void handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;

        // Updates
        virtual void updateSize() override;

        // Params
        PARAMGSU(sf::Vector2f, m_imageSize, imageSize, setImageSize, updateSize);

    private:
        sf::Sprite m_image;
    };
}
