#pragma once

#include <memory>

namespace scene
{
    // Forward declarations

    class Entity;

    //! Abstract class inherited by all components.

    class Component
    {
    public:

        //! Constuctor.
        Component(Entity& entity) : m_entity(entity) {}

        //! Default destructor.
        virtual ~Component() = default;

        //----------------------------//
        //! @name Structure callbacks
        //! @{

        //! Called whenever the entity changed its layer.
        virtual void onLayerChanged() {}

        //! @}

    protected:

        Entity& m_entity;   //!< The entity.
    };

    using ComponentPtr = std::unique_ptr<Component>;
}
