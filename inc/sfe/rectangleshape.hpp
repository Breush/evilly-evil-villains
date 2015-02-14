#pragma once

#include "scene/entity.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace sfe
{
    /** sfe::RectangleShape
     *      Just a wrapper of sf::RectangleShape to a scene::Entity
     **/

    class RectangleShape : public scene::Entity
    {
        typedef scene::Entity baseClass;

    public:
        RectangleShape() { addPart(&m_rectangleShape); }
        virtual ~RectangleShape() {}

        inline void setFillColor(const sf::Color& color) {
            m_rectangleShape.setFillColor(color);
        }

    protected:
        void update() override {
            m_rectangleShape.setSize(size());
        }

    private:
        sf::RectangleShape m_rectangleShape;
    };
}
