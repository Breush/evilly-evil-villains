#pragma once

#include "scene/entity.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <map>

// Forward declarations

enum class TextureID : uint8;

namespace nui
{
    class ReactImage : public scene::Entity
    {
        using baseClass = scene::Entity;

    protected:
        using Callback = std::function<void()>;

    public:
        ReactImage();
        virtual ~ReactImage() = default;

        // Routine
        void updateRoutine(const sf::Time& dt) override;

        // React handling
        std::wstring& getReact() { return m_react; }

        // Texture
        void setImageTexture(TextureID id);

        // React management
        void addReactFromFile(const std::string& file);
        void addReact(const std::wstring& key, sf::FloatRect rect, const Callback callback = nullptr);
        void setReactCallback(const std::wstring& key, const Callback callback);
        void setActiveReact(const std::wstring& key);

    protected:
        // Virtual
        void update() override;

        // Events
        void handleMouseButtonPressed(const sf::Mouse::Button& button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        void handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        void handleMouseLeft() override;

        // Affect visible rectangles to sprite
        void selectImageRect();
        void selectActiveRect();
        void activateReact(const std::wstring& key);

        struct ReactInfo {
            sf::FloatRect rect;
            Callback callback;
        };

        // Params
        PARAMGS(bool, m_mouseLeftDeselect, mouseLeftDeselect, setMouseLeftDeselect)
        PARAMG(bool, m_reactChanged, reactChanged)

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

