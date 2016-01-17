#pragma once

#include "sfe/sprite3d.hpp"
#include "scene/entity.hpp"
#include "tools/int.hpp"

namespace scene
{
    //! Wrapper of sfe::Sprite3D as a scene::Entity.

    class Sprite3D final : public Entity
    {
        using baseClass = Entity;

    public:

        //! Constructor.
        Sprite3D();

        //! Default destructor.
        ~Sprite3D() = default;

        std::string _name() const final { return "scene::Sprite3D"; }

        //---------------------------------------//
        //! @name Compatibily functions wrappers
        //! @{

        //! Set the texture used.
        void setTexture(const std::string& textureID);

        //! Get the texture used.
        inline const sf::Texture* getTexture() const { return m_sprite.getTexture(); }

        //! @}

        //-----------------------------//
        //! @name 3D rotation wrappers
        //! @{

        //! Set rotation around X axis.
        inline void setPitch(float pitch) { m_sprite.setPitch(pitch); }

        //! Set rotation around Y axis.
        inline void setYaw(float yaw) { m_sprite.setYaw(yaw); }

        //! Set rotation around Z axis (usual 2D rotation).
        inline void setRoll(float roll) { m_sprite.setRoll(roll); }

        //! Set the origin for the rotations.
        void setOrigin3D(const sf::Vector3f& origin3D);

        //! @}

        //--------------------------//
        //! @name 3D setup wrappers
        //! @{

        //! Set the visual depth of 3D effects.
        inline void setDepth3D(float depth) { m_sprite.setDepth(depth); }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onScaleChanges() final;

        //! @}

    private:

        //! The sprite itself.
        sfe::Sprite3D m_sprite;
    };
}
