#pragma once

#include "scene/entity.hpp"
#include "tools/int.hpp"
#include "sfe/line.hpp"
#include "nui/tablelayout.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

#include <initializer_list>
#include <vector>

// Forward declarations

namespace sfe
{
    class Label;
}

namespace nui
{
    //! A table which displays information as a list.

    class List final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        List();

        //! Default destructor.
        ~List() = default;

        //----------------//
        //! @name Columns
        //! @{

        //! Set all the titles for the columns.
        //! This will define the number of columns in the list.
        void setColumnsTitles(const std::initializer_list<std::wstring>& titles);

        //! Set how a column manage its content size.
        void setColumnAdapt(uint index, Adapt adapt);

        //! Set how a column all the lines of a column are position in their cell.
        void setColumnAlign(uint index, Align hAlign, Align vAlign);

        //! @}

        //-------------//
        //! @name Lines
        //! @{

        //! Clear all lines.
        void clearLines();

        //! Add a line to the end of the list.
        void addLine(const std::initializer_list<std::wstring>& values);

        //! @}

        //----------------------//
        //! @name Selected line
        //! @{

        //! Set the selected line.
        void selectLine(uint line);

        //! The currently selected line.
        inline uint selectedLine() const { return m_selectedLine; }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void refreshDisplay() final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        void handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        bool handleKeyboardEvent(const sf::Event& event) final;

        //! @}

        //---------------------//
        //! @name Hovered line
        //! @{

        //! Set the line being hovered.
        void hoverLine(uint line);

        //! Remove the hover highlight.
        void clearHoveredLine();

        //! Set the rect to highlight the hover.
        void setHoverRect(const sf::FloatRect& rect);

        //! @}

        //----------------------//
        //! @name Selected line
        //! @{

        //! Set the rect to highlight the selection.
        void setSelectionRect(const sf::FloatRect& rect);

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Refresh all borders to fit the current state of the table layout.
        void refreshBordersPosition();

        //! @}

        //! The information of a cell.
        struct CellInfo
        {
            std::unique_ptr<sfe::Label> label;  //!< The label of the cell.
        };

        //! The information of a line.
        struct LineInfo
        {
            std::vector<CellInfo> cells;    //!< The cells that make a line.
        };

        //! The information of a column.
        struct ColumnInfo
        {
            std::unique_ptr<sfe::Label> title;  //!< The title of this column.
            Align hAlign = Align::STANDARD;     //!< The horizontal alignment of children in this column.
            Align vAlign = Align::CENTER;       //!< The vertical alignment of children in this column.
        };

    private:

        nui::TableLayout m_table;           //!< The layout.
        std::vector<ColumnInfo> m_columns;  //!< The columns.
        std::vector<LineInfo> m_lines;      //!< The lines.

        std::vector<sfe::HLine> m_hBorders;         //!< The horizontal borders.
        std::vector<sfe::VLine> m_vBorders;         //!< The vertical borders.
        sf::RectangleShape m_selectionHighlight;    //!< The current selection is highlighted.
        sf::RectangleShape m_hoverHighlight;        //!< When the mouse is over.

        float m_headerOffset = 0.f; //!< How much the header line goes out the size.
        float m_lineHeight = 0.f;   //!< The height of a line.
        uint m_selectedLine = -1u;  //!< The selected line.
        uint m_hoveredLine = -1u;   //!< The hovered line.
    };
}

