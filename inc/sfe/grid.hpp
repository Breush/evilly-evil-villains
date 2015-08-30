#pragma once

#include "sfe/line.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

// TODO DOC

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
        virtual ~Grid() = default;

        // Virtual
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        // Setters
        void setRowsColumns(const uint& inRows, const uint& inColumns);

        // Interaction
        // Returns (-1, -1) as uint if not inside
        sf::Vector2u rowColumnFromCoords(const sf::Vector2f& coords) const;
        sf::Vector2f cellPosition(uint row, uint column) const;
        sf::Vector2f cellSize() const;

    protected:
        // Update lines positions
        void update();

        // Params
        PARAMGSU(uint, m_rows, rows, setRows, update)
        PARAMGSU(uint, m_columns, columns, setColumns, update)
        PARAMGSU(sf::Vector2f, m_size, size, setSize, update)

        //! Whether or not to draw the grid.
        PARAMGS(bool, m_visible, visible, setVisible)

    private:
        // Decorum
        std::vector<sfe::HLine> m_hBorders;
        std::vector<sfe::VLine> m_vBorders;
    };
}

