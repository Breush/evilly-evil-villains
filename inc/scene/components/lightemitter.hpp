#pragma once

#include "scene/components/component.hpp"
#include "tools/int.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

#include <vector>
#include <memory>

// Forward declarations

namespace ltbl
{
    class LightSystem;
    class LightPointEmission;
}

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

        static std::string id() noexcept { return "LightEmitter"; }

        //-------------------------//
        //! @name Lighting control
        //! @{

        //! Remove all lights.
        void clear();

        //! Add a point light.
        //! Position is relative to the entity.
        uint32 addPoint(const sf::Vector2f& position, float lightSize);

        //! Set the color of a light.
        void setColor(uint32 lightID, const sf::Color& color);

        //! @}

    protected:

        //------------------//
        //! @name Callbacks
        //! @{

        void onTransformChanged() final;
        void onLayerChanged(Layer* layer) final;

        //! @}

        //! A light information.
        struct Light
        {
            std::shared_ptr<ltbl::LightPointEmission> point;    //!< The LTBL pointer.
            sf::Vector2f position;                              //!< The relative position of the light.
        };

    private:

        ltbl::LightSystem* m_lightSystem = nullptr; //!< The light system of the layer the entity is in.
        std::vector<Light> m_lights;                //!< The light held by this entity.
    };
}
