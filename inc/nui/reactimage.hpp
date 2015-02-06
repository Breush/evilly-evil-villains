#pragma once

#include "nui/object.hpp"
#include "resources/identifiers.hpp"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <map>

namespace nui
{
    class ReactImage : public Object
    {
        typedef Object baseClass;
        typedef std::function<void()> Callback;

    public:
        ReactImage();
        virtual ~ReactImage() {}

        // Virtual
        void init() override;
        void update() override;

        // React handling
        std::wstring& getReact() { return m_react; }

        // Texture
        void setImageTexture(Textures::ID id);
        void setImageShader(Shaders::ID id);

        // React management
        void addReactFromFile(const std::string& file);
        void addReact(const std::wstring& key, sf::FloatRect rect, const Callback callback = nullptr);
        void setReactCallback(const std::wstring& key, const Callback callback);
        void setActiveReact(const std::wstring& key);

    protected:
        void activateReact(const std::wstring& key);

        // Affect visible rectangles to sprite
        void selectImageRect();
        void selectActiveRect();

        // Mouse events
        void handleMouseButtonPressed(const sf::Mouse::Button& mouseButton, const sf::Vector2f& mousePos) override;
        void handleMouseMoved(const sf::Vector2f& mousePos) override;
        void handleMouseLeft() override;

        struct ReactInfo {
            sf::FloatRect rect;
            Callback callback;
        };

        // Params
        PARAMGS(bool, m_mouseLeftDeselect, mouseLeftDeselect, setMouseLeftDeselect)


    private:
        // Target to store final image
        sf::RenderTexture m_target;

        // Image
        sf::Sprite m_image;
        sf::Sprite m_sprite;
        sf::FloatRect m_imageRect;
        sf::FloatRect* m_activeRect;

        // React
        std::map<std::wstring, ReactInfo> m_reacts;
        std::wstring m_react;
    };
}

