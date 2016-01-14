#pragma once

#include "nui/pushbutton.hpp"
#include "scene/grabbable.hpp"

#include <memory>

namespace nui
{
    //! An abstract button with an image to spawn a grabbed object.
    /*!
     *  The derived class needs to implement spawnGrabbable()
     *  and grabbableReleased().
     */

    class GrabButton : public PushButton, public scene::GrabbableSpawner
    {
        using baseClass = PushButton;

    public:

        //! Constructor.
        GrabButton();

        //! Default destructor.
        virtual ~GrabButton() = default;

        std::string _name() const final { return "nui::GrabButton"; }
    };
}
