#pragma once

#include "nui/stacker.hpp"

namespace nui
{
    //! A basic way for grouping entities horizontally.
    /*!
     *  The final size of the stacker will be computed as:
     *  - width  = \sum_i stackedEntities[i].width + paddings
     *  - height = \max_i stackedEntities[i].height
     */

    class HStacker final : public Stacker
    {
        using baseClass = Stacker;

    public:

        //! Constructor.
        HStacker();

        //! Default destructor.
        ~HStacker() = default;

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void onChildSizeChanges(scene::Entity& child) final;
        void refreshNUI(const config::NUIGuides& cNUI) final;

        void updateSize() final;

        //! @}

        //--------------------//
        //! @name Positioning
        //! @{

        //! Recomputes the positions of all stacked entities.
        void refreshChildrenPositions();

        //! Returns the vertical offset of a stackedEntity given its alignment.
        float getY(float childHeight, Align align);

        //! @}
    };
}
