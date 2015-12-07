#pragma once

#include "dungeon/inter.hpp"
#include "dungeon/databases/facilitiesdb.hpp"
#include "nui/grabbutton.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

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
        FacilityGrabButton(const FacilityData& facilityData, std::wstring facilityID, Inter& inter);

        //! Default destructor.
        ~FacilityGrabButton() = default;

    protected:

        //------------------------//
        //! @name Spawn and react
        //! @{

        void grabbableMoved(scene::Entity* entity, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos) final;
        void grabbableButtonPressed(scene::Entity* entity, const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos) final;
        std::unique_ptr<scene::Grabbable> spawnGrabbable() final;

        //! Refresh the grabbable look and the facility created.
        void setGrabbableFacilityID(const std::wstring& facilityID);

        //! Get current link.
        const Link& getCurrentExplicitLink() const;

        //! @}

    private:

        Inter& m_inter;             //!< Reference to the dungeon inter.

        const FacilityData& m_facilityData; //!< Reference to the data.
        std::wstring m_facilityID;          //!< The identifier for the facility.

        // Grabbable
        std::wstring m_grabbableFacilityID;                 //!< The identifier for the facility to create when the grabbable is going.
        std::string m_textureID;                            //!< The texture of the grabbable and button.

        // Explicit links
        sf::Vector2u m_explicitLinkCoords;  //!< Memory for the coords to link to.
        uint m_explicitLinksCount = 0u;     //!< Number of explicit links.
        uint m_explicitLinksDone = 0u;      //!< How many explicit links have we done so far.
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
        FacilityGrabbable(scene::GrabbableSpawner& spawner, const std::string& textureID);

        //! Default destructor.
        ~FacilityGrabbable() = default;

        std::string _name() const final { return "dungeon::FacilityGrabbable"; }

    private:

        //! Holding the image of the grabbable.
        sf::RectangleShape m_sprite;
    };
}

