#pragma once

#include "drawables/line.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

namespace sfe
{
    class HLine;
    class VLine;
}

namespace sfe
{
    //----- Grid -----//

    class Grid : public sf::Drawable, public sf::Transformable
    {
    public:
        Grid();
        virtual ~Grid() {}

        // Virtual
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        // Setters
        void setRowsColumns(const uint& inRows, const uint& inColumns);

        // Interaction
        // Returns (-1, -1) as uint if not inside
        sf::Vector2u rowColumnFromCoords(const sf::Vector2f& coords);

    protected:
        // Update lines positions
        void update();

        // Params
        PARAMGSU(uint, m_rows, rows, setRows, update)
        PARAMGSU(uint, m_columns, columns, setColumns, update)
        PARAMGSU(sf::Vector2f, m_size, size, setSize, update)

    private:
        // Decorum
        std::vector<sfe::HLine> m_hBorders;
        std::vector<sfe::VLine> m_vBorders;
    };
}

