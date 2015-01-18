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
        void handleMouseEvent(const sf::Event& event) override;

        // React handling
        std::string& getReact() { return m_react; }

        // Texture
        void setImageTexture(Textures::ID id);
        void setImageShader(Shaders::ID id);

        // React management
        void addReactFromFile(const std::string& file);
        void addReact(const std::string& key, sf::FloatRect rect, const Callback callback = nullptr);
        void setReactCallback(const std::string& key, const Callback callback);
        void setActiveReact(const std::string& key);

    protected:
        void activateReact(const std::string& key);

        // Affect visible rectangles to sprite
        void selectImageRect();
        void selectActiveRect();

        // Mouse events
        void handleMousePressed(const sf::Event& event);
        void handleMouseMoved(const sf::Event& event);
        void handleMouseLeft();

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
        std::map<std::string, ReactInfo> m_reacts;
        std::string m_react;
    };
}

