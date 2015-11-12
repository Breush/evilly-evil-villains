#pragma once

#include "dungeon/detectentity.hpp"
#include "dungeon/elementdata.hpp"
#include "context/event.hpp"
#include "nui/grabbutton.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace dungeon
{
    // Forward declarations

    class Inter;

    //! A generic facility interface.

    class Facility : public DetectEntity, public context::EventReceiver
    {
    public:

        //! Constructor.
        //! Set the reference to the room in data.
        Facility(const sf::Vector2u& coords, ElementData& elementdata, dungeon::Inter& inter)
            : m_coords(coords)
            , m_inter(inter)
            , m_elementdata(elementdata)
        {
            setDetectable(false);
        }

        //! Default destructor.
        virtual ~Facility() = default;

        virtual std::string _name() const override = 0;
        std::string detectKey() const final { return "facility"; }

    protected:

        //---------------//
        //! @name Events
        //! @{

        virtual void receive(const context::Event& event) override = 0;

        //! @}

        //------------------//
        //! @name Usability
        //! @{

        // virtual bool isUsable() const = 0;

        //! @}

    protected:

        sf::Vector2u m_coords;          //!< The room in which the facility is set.
        dungeon::Inter& m_inter;        //!< To be able to interact with nearby elements.
        ElementData& m_elementdata;     //!< The data corresponding to the facility.
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
        FacilityGrabButton(const std::wstring& text, std::wstring facilityID);

        //! Default destructor.
        ~FacilityGrabButton() = default;

    protected:

        //------------------------//
        //! @name Spawn and react
        //! @{

        void grabbableButtonPressed(Entity* entity, const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos) final;
        std::unique_ptr<scene::Grabbable> spawnGrabbable() final;

        //! @}

    private:

        std::string m_textureID;    //!< The texture of the grabbable and button.
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
        FacilityGrabbable(scene::GrabbableSpawner& spawner, const std::string& textureID);

        //! Default destructor.
        ~FacilityGrabbable() = default;

        std::string _name() const final { return "dungeon::FacilityGrabbable"; }

    private:

        //! Holding the image of the grabbable.
        sf::RectangleShape m_sprite;
    };
}

