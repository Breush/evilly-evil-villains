#pragma once

#include "scene/grabbable.hpp"
#include "scene/entity.hpp"
#include "scene/scene.hpp"
#include "scene/layer.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace scene
{
    /** scene::Graph
     *      Manage focus and hovering through different layers
     **/

    class Graph final : public sf::Drawable
    {
    public:
        Graph();
        ~Graph() = default;

        // Routine
        void update(const sf::Time& dt);
        void handleEvent(const sf::Event& event);
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        //! Reset the scene's views to current screen status.
        void refreshDisplay();

        // Access layers
        Layer& nuiLayer() { return m_nuiLayer; }
        Scene& scene() { return m_scene; }
        const sf::View& viewFromLayerRoot(const Entity* root) const;

        // Focusing system
        void setFocusedEntity(Entity* focusedEntity);
        void updateFocusSprite();

        // Entity callbacks
        void detachedEntity(Entity* entity);
        void attachedEntity(Entity* entity);

        // Grabbing object
        void setGrabbable(std::unique_ptr<Grabbable> grabbable);
        Entity* entityFromPosition(const sf::Vector2i& mousePos, sf::Vector2f& viewPos); //!< viewPos will be set to the position in the entity view

    protected:
        // Mouse detection
        void drawMouseDetector(sf::RenderTarget& target, sf::RenderStates states) const;
        Entity* handleMouseEvent(const sf::Event& event);
        void setHoveredEntity(Entity* hoveredEntity);

        // Focusing system
        void focusHandleEvent(const sf::Event& event);

        // Grabbing object
        void grabbableHandleMouseEvent(const sf::Event& event);

        // View manipulation
        void handleMouseWheelPressedEvent(const sf::Event& event);
        void handleMouseWheelReleasedEvent(const sf::Event& event);
        void handleMouseWheelMovedEvent(const sf::Event& event);
        bool handleMouseMovedEvent(const sf::Event& event);

        sf::Vector2f nuiPosition(sf::Vector2i mousePosition = {-1, -1});

        // Params - TODO Why are these params?
        PARAMG(sf::Shader*, m_focusShader, focusShader)
        PARAMG(sf::RectangleShape, m_focusShape, focusShape)

    private:
        // Layers
        Scene m_scene;
        Layer m_nuiLayer;

        // Mouse detection
        Entity* m_hoveredEntity = nullptr;

        // Grabbing
        bool m_grabbing = false; //!< For views
        std::unique_ptr<Grabbable> m_grabbable = nullptr; //!< For a specific grabbed object

        // Focusing system
        Entity* m_focusedEntity = nullptr;
        float m_focusAnimation = 0.f;
    };
}
