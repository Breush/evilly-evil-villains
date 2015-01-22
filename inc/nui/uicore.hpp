#pragma once

#include "states/state.hpp"
#include "resources/identifiers.hpp"
#include "tools/param.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

#include <list>
#include <map>

namespace sf
{
    class RenderTexture;
    class RenderWindow;
    class Drawable;
    class VertexArray;
}

namespace nui
{
    class Object;

    class uiCore : public sf::Drawable
    {
    public:
        uiCore();
        ~uiCore();

        // Main functions called by states
        void refresh();
        void update(sf::Time dt);
        void handleEvent(const sf::Event& event);
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        // Children management
        void add(Object* child);
        void addToDetectMap(Object* object);

        // Focusing system
        void setFocusedChild(Object* inFocusedChild);
        void forgetFocusedChild();
        void rememberFocusedChild();

    protected:
        // Detection buffer methods
        void drawDetectImage();
        uint8 getDetectValue(const sf::Vector2u& pos) const;
        uint8 getDetectValue(const uint& x, const uint& y) const;

        // Focusing system
        void updateFocusSprite();
        void manageFocusedChild(const sf::Event& event);

        // Hovering system
        void setHoveredChild(Object* inHoveredChild);

        // Params
        PARAMGNR(const sf::Vector2f&, m_viewSize, viewSize)
        PARAMG(Object*, m_hoveredChild, hoveredChild)
        PARAMG(Object*, m_focusedChild, focusedChild)
        PARAMGS(sf::IntRect, m_focusRect, focusRect, setFocusRect)

    private:
        // Children management
        std::list<Object*> m_children;

        // Focusing system
        Object* m_forgottenFocusedChild;
        sf::Sprite m_focusSprite;
        int m_focusAnimation;

        // Hovering system
        sf::Color m_detectColor;
        sf::Image m_detectImage;
        sf::RenderTexture m_detectTarget;
        std::map<uint8, Object*> m_detectMap;
    };
}
