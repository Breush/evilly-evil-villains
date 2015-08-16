#pragma once

#include "scene/entity.hpp"
#include "tools/int.hpp"
#include "nui/enums.hpp"

#include <initializer_list>
#include <map>

namespace nui
{
    //! A layout to display other entities in a table way.

    class TableLayout final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        TableLayout();

        //! Default destructor.
        ~TableLayout() = default;

        //------------------//
        //! @name Structure
        //! @{

        //! Set the dimensions of the table.
        /*!
         *  If a dimension is set to 0u, then it is automatically calculated
         *  using the provided step. It corresponds to the fixed space between
         *  two rows/columns, the adapt of these rows/columns will be set to FIXED.
         */
        void setDimensions(uint rows, uint cols,
                           float rowsStep = -1.f, float colsStep = -1.f);

        //! Set how a row adapts to its content.
        //! If FIXED, a param should be given.
        void setRowAdapt(uint row, Adapt adapt, float param = -1.f);

        //! Set how a column adapts to its content.
        //! If FIXED, a param should be given.
        void setColAdapt(uint col, Adapt adapt, float param = -1.f);

        //! Set auto-padding to false and set the padding to the given value.
        //! Pass a negative value to reset auto-padding.
        void overridePadding(float hPadding, float vPadding);

        //! Returns the starting y coordinate (with no padding) for the specified row.
        //! If row is set to the total of rows or -1, then it returns the end of the table.
        float rowOffset(uint row);

        //! Returns the starting x coordinate (with no padding) for the specified column.
        //! If col is set to the total of columns or -1, then it returns the end of the table.
        float colOffset(uint col);

        //! @}

        //-----------------//
        //! @name Children
        //! @{

        //! Set the child in position (row, col).
        /*!
         *  The child will be attached.
         *  If a previous child was set in same position,
         *  it is replaced and detached.
         */
        void setChild(uint row, uint col, scene::Entity& child,
                      Align hAlign = Align::STANDARD, Align vAlign = Align::CENTER);

        //! Set the child alignment in position (row, col).
        void setChildAlign(uint row, uint col, Align hAlign, Align vAlign);

        //! Remove all children and detached them.
        void removeChildren();

        //! Remove the child in position (row, col).
        void removeChild(uint row, uint col);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void onChildSizeChanges(scene::Entity& child) final;
        void refreshDisplay() final;

        //! @}

        //------------------//
        //! @name Structure
        //! @{

        //! Returns the max child height in the given row.
        float maxChildHeightInRow(uint row);

        //! Returns the max child width in the given column.
        float maxChildWidthInCol(uint col);

        //! Set the child position (if exists) given its current align state.
        void positionChild(uint row, uint col, float x, float y);

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Refresh the dimensions given the current size.
        void refreshDimensions();

        //! Refresh the positions of all children.
        void refreshChildrenPosition();

        //! Refresh the width of all columns.
        void refreshColsSize();

        //! Refresh the height of all rows.
        void refreshRowsSize();

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! Whether the horizontal padding should be computed automatically.
        PARAMGSU(bool, m_hPaddingAuto, hPaddingAuto, setHPaddingAuto, refreshDisplay)

        //! Whether the vertical padding should be computed automatically.
        PARAMGSU(bool, m_vPaddingAuto, vPaddingAuto, setVPaddingAuto, refreshDisplay)

        //! @}

        //! The children information.
        struct ChildInfo
        {
            scene::Entity& entity;
            Align hAlign;
            Align vAlign;
        };

        //! How a row acts.
        struct RowInfo
        {
            Adapt adapt = Adapt::FILL;  //!< How the row computes it size.
            float height;               //!< The size of the row.
        };

        //! How a column acts.
        struct ColInfo
        {
            Adapt adapt = Adapt::FILL;  //!< How the column computes it size.
            float width;                //!< The size of the column.
        };

    private:

        uint m_colsDimension = 0u;  //!< The number of rows.
        uint m_rowsDimension = 0u;  //!< The number of columns.

        float m_colsStep = -1.f;    //!< The fixed step between two columns (if enabled).
        float m_rowsStep = -1.f;    //!< The fixed step between two rows (if enabled).

        std::map<std::pair<uint, uint>, ChildInfo> m_children;  //!< The table of all children.
        std::vector<RowInfo> m_rows;    //!< The rows' data.
        std::vector<ColInfo> m_cols;    //!< The columns' data.

        float m_hPadding;   //!< The horizontal padding.
        float m_vPadding;   //!< The vertical padding.
    };
}

