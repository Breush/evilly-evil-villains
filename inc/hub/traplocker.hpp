#pragma once

#include "scene/entity.hpp"
#include "scene/wrappers/richlabel.hpp"
#include "scene/wrappers/animatedsprite.hpp"
#include "scene/wrappers/rectangleshape.hpp"

// Forward declarations

namespace dungeon
{
    class TrapData;
}

namespace hub
{
    //! A nice box that controls lock state of a trap.

    class TrapLocker final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        TrapLocker();

        //! Default destructor.
        ~TrapLocker() = default;

        std::string _name() const final { return "hub::TrapLocker"; }

        //----------------//
        //! @name Control
        //! @{

        //! Define the trap to display.
        void setSource(const std::wstring& trapID, const dungeon::TrapData& trapData);

        //! Is the trap currently locked?
        void setLocked(bool locked);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Refresh the design from the current locking.
        void refreshFromLocking();

        //! @}

    private:

        scene::RichLabel m_name;            //!< Title of the box.
        scene::AnimatedSprite m_sprite;     //!< The trap displayed.
        scene::RectangleShape m_rectangle;  //!< Border and rectangle over.
        scene::RichLabel m_lockedText;      //!< The locked text.

        // Room part
        sf::Vector2f m_roomTextureSize;     //!< The reference texture size.
        sf::RectangleShape m_room;          //!< The room part.

        // Locking state
        bool m_locked = true;           //!< Is it in a locked state?
    };
}
