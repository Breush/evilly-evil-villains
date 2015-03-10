#pragma once

#include "scene/grabbable.hpp"
#include "scene/entity.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <array>

// Forward declarations

namespace sf
{
    class View;
}

// Enums

namespace LayerID {
    enum type {
        DUNGEON_DESIGN  = 0, //!< For dungeon design mode
        NUI             = 1, //!< Keep it at the end, will be over everything
        COUNT           = 2, //!< Numbers of layers, keep it last
    };
}

namespace scene
{
    /** scene::Graph
     *      Manage focus and hovering through different layers
     **/

    class Graph : public sf::Drawable
    {
    public:
        Graph();
        virtual ~Graph() = default;

        // Routine
        void update(const sf::Time& dt);
        void handleEvent(const sf::Event& event);
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        // Access layers
        Entity& operator[](LayerID::type layerID) { return m_layers[static_cast<std::size_t>(layerID)].root; }
        const sf::View& viewFromLayerRoot(const Entity* root) const;

        // Focusing system
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
        void setFocusedEntity(Entity* focusedEntity);
        void focusHandleEvent(const sf::Event& event);

        // Grabbing object
        void grabbableHandleMouseEvent(const sf::Event& event);

        // View manipulation
        void handleMouseWheelPressedEvent(const sf::Event& event);
        void handleMouseWheelReleasedEvent(const sf::Event& event);
        void handleMouseWheelMovedEvent(const sf::Event& event);
        bool handleMouseMovedEvent(const sf::Event& event);

        // Layer
        struct Layer {
            Entity root;
            sf::View* view;
            bool manipulable;
            sf::Vector2f grabbingPosition;
        };

        // Params - TODO Why are these params?
        PARAMG(sf::Shader*, m_focusShader, focusShader)
        PARAMG(sf::RectangleShape, m_focusShape, focusShape)

    private:
        // Layers
        std::array<Layer, static_cast<std::size_t>(LayerID::COUNT)> m_layers;

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
