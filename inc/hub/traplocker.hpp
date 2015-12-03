#pragma once

#include "scene/entity.hpp"
#include "scene/wrappers/richlabel.hpp"
#include "scene/wrappers/animatedsprite.hpp"
#include "scene/wrappers/rectangleshape.hpp"

// Forward declarations

namespace dungeon
{
    class Data;
    class TrapGeneric;
}

namespace hub
{
    //! A nice box that controls lock state of a trap.

    class TrapLocker final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        TrapLocker(dungeon::Data& data);

        //! Default destructor.
        ~TrapLocker() = default;

        std::string _name() const final { return "hub::TrapLocker"; }

        //----------------//
        //! @name Control
        //! @{

        //! Define the trap to display.
        void setSource(const std::wstring& trapID, const dungeon::TrapGeneric& trapGeneric);

        //! Is the trap currently locked?
        void setLocked(bool locked);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Refresh the design from the current locking.
        void refreshFromLocking();

        //! @}

    private:

        dungeon::Data& m_data;                                  //!< Current dungeon data reference.
        const dungeon::TrapGeneric* m_trapGeneric = nullptr;    //!< Current trap generic reference.
        std::wstring m_trapID;                                  //!< Current trap ID.

        scene::RichLabel m_name;                //!< Title of the box.
        scene::AnimatedSprite m_sprite;         //!< The trap displayed.
        scene::RectangleShape m_rectangle;      //!< Border and rectangle over.
        scene::RichLabel m_lockedText;          //!< The locked text.
        scene::RectangleShape m_lockedIcon;     //!< A closed lock image.
        scene::RichLabel m_lockedCostText;      //!< The text for the cost to unlock.
        scene::RectangleShape m_lockedDoshIcon; //!< The dosh image for the cost.

        // Room part
        sf::Vector2f m_roomTextureSize;     //!< The reference texture size.
        sf::RectangleShape m_room;          //!< The room part.

        // Locking state
        bool m_locked = true;           //!< Is it in a locked state?
    };
}
