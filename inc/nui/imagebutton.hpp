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
        virtual ~ImageButton() = default;

        // Action
        void setImage(TextureID imageID);
        void setVisual(const std::wstring& text, TextureID imageID, const sf::Vector2f& inImageSize);

    protected:
        // Virtual
        void update() final;

        // Mouse events
        void handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;

        // Updates
        void updateSize() final;

        // Params
        PARAMGSU(sf::Vector2f, m_imageSize, imageSize, setImageSize, updateSize);

    private:
        sf::Sprite m_image;
    };
}
