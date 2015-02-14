#pragma once

#include "scene/entity.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <array>

namespace Layers {
    enum ID {
        DUNGEON_DESIGN,
        NUI,            //< Keep it last, to be over everything
        COUNT = 2,
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
        virtual ~Graph() {}

        // Routine
        void update(const sf::Time& dt);
        void handleEvent(const sf::Event& event);
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        // Access layers
        Entity& operator[](Layers::ID layerID) { return m_layers[layerID].root; }
        const sf::View& viewFromLayerRoot(const Entity* root) const;

        // Focusing system
        void updateFocusSprite();

        // Entity callbacks
        void detachedEntity(Entity* entity);
        void attachedEntity(Entity* entity);

    protected:
        // Mouse detection
        void drawMouseDetector(sf::RenderTarget& target, sf::RenderStates states) const;
        Entity* handleMouseEvent(const sf::Event& event);
        void setHoveredEntity(Entity* hoveredEntity);
        Entity* entityFromPosition(const sf::Vector2i& mousePos, sf::Vector2f& viewPos); //< viewPos will be set to the position in the entity view

        // Focusing system
        void setFocusedEntity(Entity* focusedEntity);
        void focusHandleEvent(const sf::Event& event);

        // Layer
        struct Layer {
            Entity root;
            sf::View* view;
        };

        // Params
        PARAMG(sf::Shader*, m_focusShader, focusShader)
        PARAMG(sf::RectangleShape, m_focusShape, focusShape)

    private:

        // Layers
        std::array<Layer, Layers::COUNT> m_layers;

        // Mouse detection
        Entity* m_hoveredEntity;

        // Focusing system
        Entity* m_focusedEntity;
        float m_focusAnimation;
    };
}
