#pragma once

#include "context/component.hpp"

#include <memory>

#include <SFML/Graphics/RenderTarget.hpp>

namespace scene
{
    // Forward declarations

    class Entity;
    class Layer;

    //! Abstract class designed to be the interface of all scene components.

    class Component : public context::Component
    {
    public:

        //! Constuctor.
        Component(Entity& entity) : m_entity(entity) {}

        //! Default destructor.
        virtual ~Component() = default;

        //----------------------------//
        //! @name Structure callbacks
        //! @{

        //! Called whenever the entity is drawn.
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {}

        //! Called whenever the entity is drawn, with a clip area.
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states, const sf::FloatRect& clipArea) const {}

        //! Called whenever the entity's size changed.
        virtual void onSizeChanged() {}

        //! Called whenever the entity's transform changed.
        virtual void onTransformChanged() {}

        //! Called whenever the entity changed its layer.
        virtual void onLayerChanged(Layer* layer) {}

        //! @}

    protected:

        Entity& m_entity;   //!< The entity.
    };

    using ComponentPtr = std::unique_ptr<Component>;
}
