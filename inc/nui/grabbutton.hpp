#pragma once

#include "nui/imagebutton.hpp"
#include "scene/grabbable.hpp"

#include <memory>

namespace nui
{
    class GrabButton : public ImageButton, public scene::GrabbableSpawner
    {
        using baseClass = ImageButton;

    public:
        GrabButton();
        virtual ~GrabButton() = default;

        virtual std::unique_ptr<scene::Grabbable> spawnGrabbable() override { return nullptr; } // FIXME Should be pure virtual

    protected:
        // Mouse events
        virtual void handleMouseButtonPressed(const sf::Mouse::Button& button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
    };
}
