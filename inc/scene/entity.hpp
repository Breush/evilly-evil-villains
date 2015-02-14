#pragma once

#include "sfe/lerpable.hpp"
#include "resources/identifiers.hpp"
#include "tools/param.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <list>

// Forward declarations
namespace sf
{
    class Shader;
}

namespace scene
{
    // Forward declarations
    class Graph;

    /** scene::Entity
     *      Base for every drawable object
     **/

    class Entity : public sf::Drawable, public sf::Transformable
    {
        friend class Graph;

    public:
        Entity(bool isLerpable = false);
        virtual ~Entity() {}

        // Components
        sfe::Lerpable* lerpable() { return m_lerpable.get(); }

        // Routine
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void update(const sf::Time& dt);

        // Children management
        std::list<Entity*>& children() { return m_children; }
        void attachChild(Entity& child);
        void detachChild(Entity& child);
        Entity* firstOver(const sf::Vector2f& position);

        // Events - TODO Can be given to Graph directly
        virtual void handleGlobalEvent(const sf::Event& event) {}

        // Local actions
        void setSize(const sf::Vector2f& resize);
        void setShader(Shaders::ID shaderID);

    protected:
        // Virtual
        virtual void update() {} //< Called whenever size or a child changed if callParentOnXXXChanges is on

        // Events - mouse-related are called if entity is detectable
        virtual void handleKeyboardEvent(const sf::Event& event) {}
        virtual void handleJoystickEvent(const sf::Event& event) {}
        virtual void handleMouseMoved(const sf::Vector2f& mousePos) {}
        virtual void handleMouseButtonPressed(const sf::Mouse::Button& mouseButton, const sf::Vector2f& mousePos) {}
        virtual void handleMouseButtonReleased(const sf::Mouse::Button& mouseButton, const sf::Vector2f& mousePos) {}
        virtual void handleMouseWheelMoved(const int& delta, const sf::Vector2f& mousePos) {}
        virtual void handleMouseLeft() {}

        // Routine
        void drawParts(sf::RenderTarget& target, sf::RenderStates states) const;
        virtual void updateRoutine(const sf::Time& dt) {}
        void updateLerpable(const sf::Time& dt);
        void updateChanges();

        // Sub-parts of drawing
        struct Part {
            sf::Drawable*   drawable;
            sf::Shader*     shader;
            bool            clipping;
            sf::FloatRect   clippingRect;
        };

        // Parts management
        void clearParts();
        void resetPartsShader();
        void removePart(sf::Drawable* drawable);
        void setPartShader(sf::Drawable* drawable, Shaders::ID shader);
        void setPartClippingRect(sf::Drawable* drawable, const sf::FloatRect& clippingRect);
        void addPart(sf::Drawable* drawable, Shaders::ID shader = Shaders::NONE);

        // Refresh on local changes
        void refreshFromLocal();
        void refreshFromLocalPosition();
        void refreshFromLocalRotation();
        void refreshFromLocalScale();
        void refreshFocusRect();
        void refreshCentering();
        void refreshDepthOrder();
        void refreshChildrenOrder();

        // Focusing system
        void setFocused(bool inFocused);
        void setFocusRect(const sf::FloatRect& inFocusRect);
        Entity* lastDescendant();
        Entity* closestPreviousSibling();
        Entity* previousFocusable();
        Entity* closestNextSibling();
        Entity* nextFocusableChild();
        Entity* nextFocusable();

        // Delayed operations
        void markForVisible(bool visible);

        // Graph
        void setGraph(Graph* graph);
        Entity* root();

        // Params
        PARAMG(Graph*, m_graph, graph)
        PARAMGSU(float, m_depth, depth, setDepth, refreshDepthOrder)
        PARAMGSU(Entity*, m_parent, parent, setParent, refreshFromLocal)

        PARAMGSU(sf::Vector2f, m_localPosition, localPosition, setLocalPosition, refreshFromLocalPosition)
        PARAMGSU(float, m_localRotation, localRotation, setLocalRotation, refreshFromLocalRotation)
        PARAMGSU(sf::Vector2f, m_localScale, localScale, setLocalScale, refreshFromLocalScale)
        PARAMG(sf::Vector2f, m_size, size)

        PARAMGSU(bool, m_centered, centered, setCentered, refreshCentering)
        PARAMGS(bool, m_visible, visible, setVisible)
        PARAMGS(bool, m_detectable, detectable, setDetectable)
        PARAMGS(bool, m_focusable, focusable, setFocusable)

        PARAMG(sf::FloatRect, m_focusRect, focusRect)
        PARAMGS(bool, m_focusOwned, focusOwned, setFocusOwned)

    private:
        // Children management
        std::list<Entity*> m_children;

        // Drawing
        sf::Shader* m_shader;

        // Focusing
        bool m_focused;

        // Local manipulation
        bool m_sizeChanges;
        bool m_localChanges;
        bool m_callParentOnSizeChanges;
        bool m_callParentOnLocalChanges;

        // Parts management
        std::vector<Part> m_parts;

        // Delayed operations
        bool m_markedForVisible;
        bool m_visibleMark;

        // Components
        std::unique_ptr<sfe::Lerpable> m_lerpable;
    };
}
