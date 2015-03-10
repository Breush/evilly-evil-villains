#pragma once

#include "sfe/lerpable.hpp"
#include "tools/param.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <list>

// Forward declarations

enum class ShaderID : uint8;

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
        virtual ~Entity() = default;

        // Components
        sfe::Lerpable* lerpable() noexcept { return m_lerpable.get(); }

        // Routine
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void update(const sf::Time& dt);

        // Children management
        std::list<Entity*>& children() noexcept { return m_children; }
        void attachChild(Entity& child);
        void detachChild(Entity& child);
        Entity* firstOver(const sf::Vector2f& position);

        // Local actions
        void markForVisible(bool visible);
        void setSize(const sf::Vector2f& resize);
        void setShader(ShaderID shaderID);

    protected:
        // Virtual
        virtual void update() {} //< Called whenever size or a child changed if callParentOnXXXChanges is on
        virtual void updateRoutine(const sf::Time& dt) {}

        // Events - mouse-related are called if entity is detectable
        virtual void handleKeyboardEvent(const sf::Event& event) {}
        virtual void handleJoystickEvent(const sf::Event& event) {}
        virtual void handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) {}
        virtual void handleMouseButtonPressed(const sf::Mouse::Button& button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) {}
        virtual void handleMouseButtonReleased(const sf::Mouse::Button& button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) {}
        virtual void handleMouseWheelMoved(const int& delta, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) {}
        virtual void handleMouseLeft() {}

        // Routine
        void drawParts(sf::RenderTarget& target, sf::RenderStates states) const;
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
        void addPart(sf::Drawable* drawable);
        void removePart(sf::Drawable* drawable);
        void resetPartsShader();
        void setPartShader(sf::Drawable* drawable, ShaderID shader);
        void setPartClippingRect(sf::Drawable* drawable, const sf::FloatRect& clippingRect);

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

        // Utilities
        sf::Vector2i displayedPixel(const sf::Vector2f& position);

        // Graph
        void setGraph(Graph* graph);
        Entity* root();

        // Params
        PARAMG(Graph*, m_graph, graph)
        PARAMGSU(Entity*, m_parent, parent, setParent, refreshFromLocal)
        PARAMGSU(float, m_depth, depth, setDepth, refreshDepthOrder)

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
        sf::Shader* m_shader = nullptr;

        // Focusing
        bool m_focused = false;

        // Local manipulation
        bool m_sizeChanges = true;
        bool m_localChanges = true;
        bool m_callParentOnSizeChanges = true;
        bool m_callParentOnLocalChanges = false;

        // Parts management
        std::vector<Part> m_parts;

        // Delayed operations
        bool m_markedForVisible = false;
        bool m_visibleMark;

        // Components
        std::unique_ptr<sfe::Lerpable> m_lerpable = nullptr;
    };
}
