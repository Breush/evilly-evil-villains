#pragma once

#include "nui/stacker.hpp"

namespace nui
{
    //! A basic way for grouping entities vertically.
    /*!
     *  The final size of the stacker will be computed as:
     *  - width  = \max_i stackedEntities[i].width
     *  - height = \sum_i stackedEntities[i].height + paddings
     */

    class VStacker final : public Stacker
    {
        using baseClass = Stacker;

    public:

        //! Constructor.
        VStacker();

        //! Default destructor.
        ~VStacker() = default;

        std::string _name() const final { return "nui::VStacker"; }

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

        //! Returns the horizontal offset of a stackedEntity given its alignment.
        float getX(float childWidth, Align align);

        //! @}
    };
}
