#pragma once

#include "scene/components/component.hpp"
#include "tools/int.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

#include <ltbl/lighting/LightSystem.h>

namespace scene
{
    //! The entity can react to light into the layer's light system using a normals map.

    class LightNormals final : public Component
    {
        using baseClass = Component;

    public:

        //! Constructor.
        LightNormals(Entity& entity);

        //! Destructor.
        ~LightNormals();

        static std::string id() noexcept { return "LightNormals"; }

        //----------------//
        //! @name Routine
        //! @{

        void update(const sf::Time& dt);

        //! @}

        //------------------------//
        //! @name Normals control
        //! @{

        //! Set the texture to be used for normals.
        void setNormalsTexture(const std::string& textureID);

        //! @}

    protected:

        //------------------//
        //! @name Callbacks
        //! @{

        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;
        void draw(sf::RenderTarget& target, sf::RenderStates states, const sf::FloatRect& clipArea) const final;

        void onSizeChanged() final;
        void onLayerChanged(Layer* layer) final;

        //! @}

    private:

        ltbl::LightSystem* m_lightSystem = nullptr; //!< The light system of the layer the entity is in.
        sf::RectangleShape m_shape;                 //!< The normals to be drawn.
        bool m_drawn = false;                       //!< Have we already drawn this?
    };
}
