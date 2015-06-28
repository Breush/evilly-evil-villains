#pragma once

#include "scene/entity.hpp"
#include "dungeon/event.hpp"
#include "nui/grabbutton.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

// Forward declarations

enum class TextureID : uint8;

namespace dungeon
{
    //! A FacilityGrabbable spawner.
    /*!
     *  Implements the scene::GrabbableSpawner for a dungeon facility.
     *  Will launch a callback on dungeon::Inter::setRoomFacility().
     */

    class FacilityGrabButton final : public nui::GrabButton
    {
        using baseClass = nui::GrabButton;

    public:

        //! Constructor, affecting texture and look.
        FacilityGrabButton(const std::wstring& text, TextureID textureID, std::wstring facilityID);

        //! Default destructor.
        ~FacilityGrabButton() = default;

    protected:

        //------------------------//
        //! @name Spawn and react
        //! @{

        void grabbableReleased(Entity* entity, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos) final;
        std::unique_ptr<scene::Grabbable> spawnGrabbable() final;

        //! @}

    private:

        TextureID m_textureID;      //!< The texture of the grabbable and button.
        std::wstring m_facilityID;  //!< The facility to be constructed in the dungeon inter.
    };

    //! A facility temporary object.
    /*!
     *  Implements the scene::Grabbable for the a dungeon facility.
     */

    class FacilityGrabbable final : public scene::Grabbable
    {
        using baseClass = scene::Grabbable;

    public:

        //! Constructor, affecting texture and look.
        FacilityGrabbable(scene::GrabbableSpawner& spawner, TextureID textureID);

        //! Default destructor.
        ~FacilityGrabbable() = default;

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

