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

        // Virtual
        virtual void init() override;
        virtual void update() override;

        // Action
        void setImage(Textures::ID imageID);
        void setVisual(const std::wstring& text, Textures::ID imageID, const sf::Vector2f& inImageSize);

    protected:
        // Mouse events
        virtual void handleMouseMoved(const sf::Event& event, const sf::Vector2f& absPos, const sf::Vector2f& relPos) override;

        // Updates
        virtual void updateSize() override;

        // Params
        PARAMGSU(sf::Vector2f, m_imageSize, imageSize, setImageSize, updateSize);

    private:
        sf::Sprite m_image;
    };
}
