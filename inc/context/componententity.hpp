#pragma once

#include "context/component.hpp"

#include <unordered_map>
#include <string>

namespace context
{
    //! An entity that accept components.

    class ComponentEntity
    {
    public:

        //! Default constructor.
        ComponentEntity() = default;

        //! Default destructor.
        virtual ~ComponentEntity() = default;

        //-------------------//
        //! @name Components
        //! @{

        //! Does the entity have the specified component?
        template <class Component_t>
        bool hasComponent() const;

        //! Get the specified component.
        //! Return nullptr if it does not exist in the entity.
        template <class Component_t>
        Component_t* getComponent();

        //! Add the specified component.
        template <class Component_t, class... Args>
        Component_t* addComponent(Args&&... args);

        //! Remove the specified component.
        template <class Component_t>
        void removeComponent();

        //! @}

    protected:

        // Components
        std::unordered_map<std::string, Component*> m_components;   //!< All the components.
    };
}

#include "context/componententity.inl"
