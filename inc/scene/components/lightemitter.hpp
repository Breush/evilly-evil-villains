#pragma once

#include "scene/components/component.hpp"
#include "tools/int.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

namespace scene
{
    //! The entity can emit light into the layer's light system.

    class LightEmitter final : public Component
    {
        using baseClass = Component;

    public:

        //! Constructor.
        LightEmitter(Entity& entity);

        //! Destructor.
        ~LightEmitter();

        //-------------------------//
        //! @name Lighting control
        //! @{

        //! Add a point light.
        //! Position is relative to the entity.
        uint32 addPoint(const sf::Vector2f& relPos, float lightSize);

        //! Set the color of a light.
        void setColor(uint32 lightID, const sf::Color& color);

        //! @}

    protected:

        //------------------//
        //! @name Callbacks
        //! @{

        void onLayerChanged() final;

        //! @}
    };
}
