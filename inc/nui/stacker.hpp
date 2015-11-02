#pragma once

#include "scene/entity.hpp"
#include "nui/enums.hpp"

#include <list>

namespace nui
{
    //! A basic way for grouping entities.
    /*!
     *  This is a purely abstract class, use nui::VStacker or nui::HStacker,
     *  for vertical and horizontal stacking respectively.
     */

    class Stacker : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        Stacker();

        //! Default destructor.
        virtual ~Stacker() = default;

        virtual std::string _name() const override = 0;

        //----------------------------//
        //! @name Children management
        //! @{

        //! Stacks an entity to the end of the stacker.
        void stackBack(scene::Entity& entity, Align align = Align::STANDARD);

        //! Detached all entities stacked.
        void unstackAll();

        //! @}

        //----------------//
        //! @name Display
        //! @{

        //! Overwrite the default padding.
        //! Set to a negative value to reset to default.
        void setPadding(const float inPadding);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onChildDetached(scene::Entity& child) final;
        virtual void onSizeChanges() override = 0;
        virtual void onChildSizeChanges(scene::Entity& child) override = 0;

        //! Updates the total size of the stacker and reposition children.
        virtual void updateSize() = 0;

        //! @}

        //! Reference an entity and defines its positionning.
        struct Child
        {
            scene::Entity* entity;  //!< The stacked entity.
            Align align;            //!< The algnment of the stacked entity.
        };

    protected:

        std::list<Child> m_children;    //!< All stacked entities.

        // Padding
        float m_padding = -1.f;             //!< The space between two stacked entities.
        bool m_paddingOverwritten = false;  //!< Decides if the padding is controlled externally.
    };
}

