#pragma once

#include "scene/entity.hpp"
#include "dungeon/elements/elementdata.hpp"
#include "context/event.hpp"
#include "nui/grabbutton.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

// TODO Rename this file to something similar at MonsterCage

namespace dungeon
{
    //! A TrapGrabbable spawner.
    /*!
     *  Implements the scene::GrabbableSpawner for a dungeon trap.
     *  Will launch a callback on dungeon::Inter::setRoomTrap().
     */

    class TrapGrabButton final : public nui::GrabButton
    {
        using baseClass = nui::GrabButton;

    public:

        //! Constructor, affecting texture and look.
        TrapGrabButton(const std::wstring& text, std::wstring trapID);

        //! Default destructor.
        ~TrapGrabButton() = default;

    protected:

        //------------------------//
        //! @name Spawn and react
        //! @{

        void grabbableButtonReleased(scene::Entity* entity, const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos) final;
        std::unique_ptr<scene::Grabbable> spawnGrabbable() final;

        //! @}

    private:

        std::string m_textureID;    //!< The texture of the grabbable and button.
        std::wstring m_trapID;      //!< The trap to be constructed in the dungeon inter.
    };

    //! A trap temporary object.
    /*!
     *  Implements the scene::Grabbable for the a dungeon trap.
     */

    class TrapGrabbable final : public scene::Grabbable
    {
        using baseClass = scene::Grabbable;

    public:

        //! Constructor, affecting texture and look.
        TrapGrabbable(scene::GrabbableSpawner& spawner, const std::wstring& trapID);

        //! Default destructor.
        ~TrapGrabbable() = default;

        std::string _name() const final { return "dungeon::TrapGrabbable"; }

    private:

        //! Holding the image of the grabbable.
        sf::RectangleShape m_sprite;
    };
}

