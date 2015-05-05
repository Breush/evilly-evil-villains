#pragma once

#include "nui/grabbutton.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

// Forward declarations

enum class TextureID : uint8;

namespace dungeon
{
    //! The facilities.
    /*!
     *  A facility is an object/structure that helps an invader
     *  There can be only one per dungeon room.
     */
    enum FacilityID : uint8
    {
        ENTRANCE,   //!< The main entrance to the dungeon.
        LADDER,     //!< A ladder to climb upstairs.
        TREASURE,   //!< The main treasure.
        COUNT,      //!< Number of facilities. KEEP LAST.
    };

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
        FacilityGrabButton(const std::wstring& text, TextureID textureID, FacilityID facilityID);

        //! Default destructor.
        virtual ~FacilityGrabButton() = default;

    protected:

        //------------------------//
        //! @name Spawn and react
        //! @{

        void grabbableReleased(Entity* entity, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos) final;
        std::unique_ptr<scene::Grabbable> spawnGrabbable() final;

        //! @}

    private:

        TextureID m_textureID;      //!< The texture of the grabbable and button.
        FacilityID m_facilityID;    //!< The facility to be constructed in the dungeon inter.
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
        virtual ~FacilityGrabbable() = default;

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
