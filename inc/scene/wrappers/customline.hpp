#pragma once

#include "scene/entity.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace scene
{
    //! A customizable line over a scene::Entity.

    class CustomLine final : public Entity
    {
        using baseClass = Entity;

    public:

        //! Constructor.
        CustomLine();

        //! Default destructor.
        ~CustomLine() = default;

        std::string _name() const final { return "CustomLine"; }

        //----------------//
        //! @name Control
        //! @{

        //! The local positions of two points of the custom line.
        void setLimits(const sf::Vector2f& start, const sf::Vector2f& end);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;

        //! @}

    private:

        //! The rectangle shape used.
        sf::RectangleShape m_rectangleShape;
    };
}
