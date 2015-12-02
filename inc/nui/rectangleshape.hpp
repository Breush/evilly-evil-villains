#pragma once

#include "nui/entity.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace nui
{
    //! Wrapper of sf::RectangleShape as a nui::Entity.

    class RectangleShape final : public Entity
    {
        using baseClass = Entity;

    public:

        //! Constructor.
        RectangleShape();

        //! Default destructor.
        ~RectangleShape() = default;

        std::string _name() const final { return "nui::RectangleShape"; }

        //-----------------//
        //! @name Wrappers
        //! @{

        //! Wrapper to set a texture.
        void setTexture(const std::string& textureID);

        //! Wrapper to set the fill color.
        inline void setFillColor(const sf::Color& color) { m_rectangleShape.setFillColor(color); }

        //! Wrapper to set the outline color.
        inline void setOutlineColor(const sf::Color& color) { m_rectangleShape.setOutlineColor(color); }

        //! Wrapper to set the outline thickness.
        inline void setOutlineColor(const float outlineThickness) { m_rectangleShape.setOutlineThickness(outlineThickness); }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void onScaleChanges() final;

        //! @}

    private:

        //! The rectangle shape itself.
        sf::RectangleShape m_rectangleShape;
    };
}
