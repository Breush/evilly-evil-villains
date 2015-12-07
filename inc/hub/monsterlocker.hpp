#pragma once

#include "scene/entity.hpp"
#include "scene/wrappers/richlabel.hpp"
#include "scene/wrappers/animatedsprite.hpp"
#include "scene/wrappers/rectangleshape.hpp"
#include "dungeon/sidebar/costbanner.hpp"

// Forward declarations

namespace dungeon
{
    class Data;
    class MonsterGeneric;
}

namespace hub
{
    //! A nice box that controls lock state of a monster.

    class MonsterLocker final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        MonsterLocker(dungeon::Data& data);

        //! Default destructor.
        ~MonsterLocker() = default;

        std::string _name() const final { return "hub::MonsterLocker"; }

        //----------------//
        //! @name Control
        //! @{

        //! Define the trap to display.
        void setSource(const std::wstring& monsterID, const dungeon::MonsterGeneric& monsterGeneric);

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

        dungeon::Data& m_data;                                      //!< Current dungeon data reference.
        const dungeon::MonsterGeneric* m_monsterGeneric = nullptr;  //!< Current monster generic reference.
        std::wstring m_monsterID;                                   //!< Current monster ID.

        scene::RichLabel m_name;                //!< Title of the box.
        scene::AnimatedSprite m_sprite;         //!< The trap displayed.
        scene::RectangleShape m_rectangle;      //!< Border and rectangle over.
        scene::RichLabel m_lockedText;          //!< The locked text.
        scene::RectangleShape m_lockedIcon;     //!< A closed lock image.
        scene::RichLabel m_lockedCostText;      //!< The text for the cost to unlock.
        scene::RectangleShape m_lockedDoshIcon; //!< The dosh image for the cost.

        dungeon::CostBanner m_requireCostBanner;    //!< Shows the requirements.

        // Room part
        sf::Vector2f m_roomTextureSize;     //!< The reference texture size.
        sf::RectangleShape m_room;          //!< The room part.

        // Locking state
        bool m_locked = true;           //!< Is it in a locked state?
    };

    using MonsterLockerPtr = std::unique_ptr<MonsterLocker>;
}
