#pragma once

#include "tools/param.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace sfe
{
    //----- Arrow -----//

    class Arrow : public sf::Drawable, public sf::Transformable
    {
    public:
        Arrow();
        virtual ~Arrow() {}

        // Virtual
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    protected:
        // Update vertices positions or colors
        virtual void update() = 0;

        // Params
        PARAMGSU(float, m_length, length, setLength, update);

    protected:
        sf::VertexArray m_vertices;
    };

    //----- LArrow -----//

    class LArrow : public Arrow
    {
    public:
        LArrow() {}
        virtual ~LArrow() {}

    protected:
        void update() override;
    };

    //----- RArrow -----//

    class RArrow : public Arrow
    {
    public:
        RArrow() {}
        virtual ~RArrow() {}

    protected:
        void update() override;
    };
}

