#pragma once

#include "sfe/line.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

// Forward declarations

namespace sfe
{
    class HLine;
    class VLine;
}

namespace sfe
{
    //! A simple grid that can be displayed or just used to get coordinates.

    class Grid final : public sf::Drawable, public sf::Transformable
    {
    public:

        //! Constructor.
        Grid();

        //! Default destructor.
        ~Grid() = default;

        //----------------//
        //! @name Routine
        //! @{

        //! Implements sf::Drawable drawing routine.
        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

        //! @}

        //--------------------//
        //! @name Interaction
        //! @{

        //! Set the number of rows and columns.
        void setRowsColumns(const uint& inRows, const uint& inColumns);

        //! Get row/column coordinates from relative coords.
        sf::Vector2u coordsFromPosition(const sf::Vector2f& position) const;

        //! Get relative coords from row/column coords.
        sf::Vector2f cellPosition(uint row, uint column) const;

        //! Size of a cell size.
        sf::Vector2f cellSize() const;

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! How many rows in the grid.
        PARAMGSU(uint, m_rows, rows, setRows, update)

        //! How many columns in the grid.
        PARAMGSU(uint, m_columns, columns, setColumns, update)

        //! The size of the grid.
        PARAMGSU(sf::Vector2f, m_size, size, setSize, update)

        //! Whether or not to draw the grid.
        PARAMGS(bool, m_visible, visible, setVisible)

        //! @}

    protected:

        //----------------//
        //! @name Updates
        //! @{

        //! Update borders positions.
        void update();

        //! @}

    private:

        // Decorum
        std::vector<sfe::HLine> m_hBorders; //!< The horizontal borders.
        std::vector<sfe::VLine> m_vBorders; //!< The vertical borders.
    };
}

