#pragma once

#include "tools/param.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace sfe
{
    //----- Line -----//

    class Line : public sf::Drawable, public sf::Transformable
    {
    public:
        Line();
        virtual ~Line() = default;

        // Virtual
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    protected:
        // Update vertices positions
        virtual void update() = 0;

        // Params
        PARAMGSU(float, m_length, length, setLength, update);
        PARAMGSU(float, m_shade, shade, setShade, update);

    protected:
        sf::VertexArray m_vertices;
    };

    //----- HLine -----//

    class HLine : public Line
    {
    public:
        HLine() = default;
        virtual ~HLine() = default;

    protected:
        void update() override;
    };

    //----- VLine -----//

    class VLine : public Line
    {
    public:
        VLine() = default;
        virtual ~VLine() = default;

    protected:
        void update() override;
    };
}
