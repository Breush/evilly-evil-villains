#pragma once

#include "nui/grabbutton.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace dungeon
{
    //! Tools identifiers.

    enum class ToolID
    {
        ROOMS_BUILDER,
        ROOMS_DESTROYER,
        TRAPS_REMOVER,
        FACILITIES_REMOVER,
    };

    //! A ToolGrabbable spawner.
    /*!
     *  Implements the scene::GrabbableSpawner for a tool.
     */

    class ToolGrabButton final : public nui::GrabButton
    {
        using baseClass = nui::GrabButton;

    public:

        //! Constructor, affecting look.
        ToolGrabButton(const std::wstring& text, ToolID toolID);

        //! Default destructor.
        ~ToolGrabButton() = default;

    protected:

        //------------------------//
        //! @name Spawn and react
        //! @{

        void grabbableButtonPressed(Entity* entity, const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos) final;
        std::unique_ptr<scene::Grabbable> spawnGrabbable() final;

        //! @}

    private:

        std::string m_textureID;    //!< The texture of the grabbable and button.
        ToolID m_toolID;            //!< The tool.
    };

    //! A tool temporary object, when selected.
    /*!
     *  Implements the scene::Grabbable for the a tool.
     */

    class ToolGrabbable final : public scene::Grabbable
    {
        using baseClass = scene::Grabbable;

    public:

        //! Constructor, affecting texture and look.
        ToolGrabbable(scene::GrabbableSpawner& spawner, const std::string& textureID);

        //! Default destructor.
        ~ToolGrabbable() = default;

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

        //! @}

    private:

        //! Holding the image of the grabbable.
        sf::RectangleShape m_sprite;
    };
}

