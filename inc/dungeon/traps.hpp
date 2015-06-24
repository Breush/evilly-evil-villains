#pragma once

#include "scene/entity.hpp"
#include "dungeon/trapdata.hpp"
#include "dungeon/event.hpp"
#include "nui/grabbutton.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace dungeon
{
    //! A generic trap interface.
    class Trap : public scene::Entity, public EventReceiver
    {
    public:

        //! Constructor.
        //! Set the reference to the room in data.
        Trap(const sf::Vector2u& coords, TrapData& trapdata)
            : m_coords(coords)
            , m_trapdata(trapdata)
        {
            setDetectable(false);
        }

        //! Default destructor.
        virtual ~Trap() = default;

        //------------------//
        //! @name Resources
        //! @{

        //! Remove dosh from resources and return the value harvested.
        virtual uint32 harvestDosh() = 0;

        //! How much dosh is harvestable from this trap.
        virtual uint32 harvestableDosh() const = 0;

        //! @}

    protected:

        //---------------//
        //! @name Events
        //! @{

        virtual void receive(const Event& event) override = 0;

        //! @}

    protected:

        sf::Vector2u m_coords;  //!< The room in which the trap is set.
        TrapData& m_trapdata;   //!< The data corresponding to the trap.
    };

    //! A TrapGrabbable spawner.
    /*!
     *  Implements the scene::GrabbableSpawner for a dungeon facility.
     *  Will launch a callback on dungeon::Inter::setRoomFacility().
     */

    class TrapGrabButton final : public nui::GrabButton
    {
        using baseClass = nui::GrabButton;

    public:

        //! Constructor, affecting texture and look.
        TrapGrabButton(const std::wstring& text, TextureID textureID, std::wstring trapID);

        //! Default destructor.
        ~TrapGrabButton() = default;

    protected:

        //------------------------//
        //! @name Spawn and react
        //! @{

        void grabbableReleased(Entity* entity, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos) final;
        std::unique_ptr<scene::Grabbable> spawnGrabbable() final;

        //! @}

    private:

        TextureID m_textureID;      //!< The texture of the grabbable and button.
        std::wstring m_trapID;  //!< The facility to be constructed in the dungeon inter.
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
        TrapGrabbable(scene::GrabbableSpawner& spawner, TextureID textureID);

        //! Default destructor.
        ~TrapGrabbable() = default;

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

