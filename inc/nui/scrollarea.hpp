#pragma once

#include "scene/entity.hpp"

namespace nui
{
    //! An area with scroll-bars (if needed) that keeps its specific size.
    /*!
     *  One usally affects a stacker with unknown or variable size as content.
     */

    class ScrollArea final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        ScrollArea();

        //! Default destructor.
        ~ScrollArea() = default;

        //---------------------------//
        //! @name Content management
        //! @{

        //! The entity used as content.
        void setContent(scene::Entity& entity);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void onChildSizeChanges(scene::Entity& child) final;

        //! @}

        //--------------------------------//
        //! @name Internal changes update
        //!

        //! Recompute bar visibility and lengths.
        void refreshBars();

        //! Refresh clip area and position of the content.
        void refreshContentStatus();

        //! @}

    private:

        scene::Entity* m_content = nullptr; //!< The entity used as content.
        sf::Vector2f m_offset = {0.f, 0.f}; //!< The start for the visible top-left corner of the entity.
    };
}
