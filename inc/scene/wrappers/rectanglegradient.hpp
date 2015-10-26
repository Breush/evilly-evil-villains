#pragma once

#include "scene/entity.hpp"
#include "sfe/rectanglegradient.hpp"

namespace scene
{
    //! Wrapper of sfe::RectangleGradient as a scene::Entity.

    class RectangleGradient final : public Entity
    {
        using baseClass = Entity;

    public:

        //! Constructor.
        RectangleGradient();

        //! Default destructor.
        ~RectangleGradient() = default;

        std::string _name() const final { return "RectangleGradient"; }

        //-----------------//
        //! @name Wrappers
        //! @{

        //! Wrapper to set a texture.
        void setTexture(const std::string& textureID);

        //! Wrapper to set a point color.
        inline void setPointColor(const sfe::RectangleGradient::PointPosition pointPosition, const sf::Color& color)
        { m_rectangleGradient.setPointColor(pointPosition, color); }

        //! Wrapper to set the outline color.
        inline void setPointsColors(const sf::Color& topColor, const sf::Color& bottomColor)
        { m_rectangleGradient.setPointsColors(topColor, bottomColor); }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;

        //! @}

    private:

        //! The rectangle gradient itself.
        sfe::RectangleGradient m_rectangleGradient;
    };
}
