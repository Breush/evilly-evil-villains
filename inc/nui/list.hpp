#pragma once

#include "nui/entity.hpp"
#include "scene/wrappers/label.hpp"
#include "tools/int.hpp"
#include "sfe/line.hpp"
#include "nui/tablelayout.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <initializer_list>
#include <vector>

namespace nui
{
    //! A table which displays information as a list.

    class List final : public nui::Entity
    {
        using baseClass = nui::Entity;

        //! The function signature to call on validation.
        using Callback = std::function<void()>;

    public:

        //! Constructor.
        List();

        //! Default destructor.
        ~List() = default;

        std::string _name() const final { return "nui::List"; }

        //----------------//
        //! @name Columns
        //! @{

        //! Sets the callback to launch when return key is pressed or a line is double-clicked.
        void setActionCallback(const Callback& callback) { m_callback = callback; }

        //! @}

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

        //! Return true if no line in list.
        inline bool empty() const { return m_lines.empty(); }

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

        void updateRoutine(const sf::Time& dt) final;
        void onSizeChanges() final;
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
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
            std::unique_ptr<scene::Label> label;  //!< The label of the cell.
        };

        //! The information of a line.
        struct LineInfo
        {
            std::vector<CellInfo> cells;    //!< The cells that make a line.
        };

        //! The information of a column.
        struct ColumnInfo
        {
            std::unique_ptr<scene::Label> title;  //!< The title of this column.
            Align hAlign = Align::STANDARD;     //!< The horizontal alignment of children in this column.
            Align vAlign = Align::CENTER;       //!< The vertical alignment of children in this column.
        };

    private:

        nui::TableLayout m_table;           //!< The layout.
        std::vector<ColumnInfo> m_columns;  //!< The columns.
        std::vector<LineInfo> m_lines;      //!< The lines.

        // Header
        sf::Sprite m_headerLeft;            //!< The left part of the header.
        sf::Sprite m_headerRight;           //!< The right part of the header.
        sf::RectangleShape m_headerMiddle;  //!< The (repeated) middle part of the header.
        sf::Vector2f m_headerLeftSize;      //!< The header left size.

        // Fill
        sf::RectangleShape m_fillLeft;      //!< The left part of filling.
        sf::RectangleShape m_fillRight;     //!< The right part of filling.
        sf::RectangleShape m_fillMiddle;    //!< The middle part of filling.
        sf::Vector2f m_fillMiddleSize;      //!< The fill middle size.

        // Bottom
        sf::RectangleShape m_footerLeft;    //!< The left part of the footer.
        sf::RectangleShape m_footerRight;   //!< The right part of the footer.
        sf::RectangleShape m_footerMiddle;  //!< The middle part of the footer.
        sf::Vector2f m_footerRightSize;     //!< The footer right size.

        // Highlights
        std::vector<sfe::VLine> m_vBorders;         //!< The vertical borders.
        sf::RectangleShape m_selectionHighlight;    //!< The current selection is highlighted.
        sf::RectangleShape m_hoverHighlight;        //!< When the mouse is over.

        // Validation
        Callback m_callback = nullptr;      //!< The callback on validation.
        float m_doubleClickDelay = -1.f;    //!< Double-click timer, keep negative for long-passed first click.

        // Decorum
        float m_lineHeight = 0.f;   //!< The height of a line.
        uint m_selectedLine = -1u;  //!< The selected line.
        uint m_hoveredLine = -1u;   //!< The hovered line.
    };
}

