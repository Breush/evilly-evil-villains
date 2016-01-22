#pragma once

#include <memory>

#include <SFML/System/Time.hpp>

namespace scene
{
    // Forward declarations

    class Entity;
    class Layer;

    //! Abstract class designed to be the interface of all components.

    class Component
    {
    public:

        //! Constuctor.
        Component(Entity& entity) : m_entity(entity) {}

        //! Default destructor.
        virtual ~Component() = default;

        //--------------------------//
        //! @name Routine callbacks
        //! @{

        //! Regular update routine.
        virtual void update(const sf::Time& dt) {}

        //! @}

        //----------------------------//
        //! @name Structure callbacks
        //! @{

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
