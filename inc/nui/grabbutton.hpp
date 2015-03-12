#pragma once

#include "nui/imagebutton.hpp"
#include "scene/grabbable.hpp"

#include <memory>

namespace nui
{
    //! An abstract button with an image to spawn a grabbed object.
    /*!
     *  This class exists so that one can make a container (like std::vector)
     *  of pointers. The derived class needs to implement spawnGrabbable()
     *  and grabbableReleased().
     */

    class GrabButton : public ImageButton, public scene::GrabbableSpawner
    {
        using baseClass = ImageButton;

    public:

        //! Default constructor.
        GrabButton() = default;

        //! Default destructor.
        virtual ~GrabButton() = default;

    protected:

        //---------------//
        //! @name Events
        //! @{

        //! Spawn grabbable on click and set it to graph.
        void handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;

        //! @}
    };
}
