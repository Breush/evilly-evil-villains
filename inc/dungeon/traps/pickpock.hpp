#pragma once

#include "dungeon/traps.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace dungeon
{
    // Forward declarations

    class Hero;

namespace traps
{
    //!< A pick-pock steals money from passing by heroes.

    class PickPock final : public Trap
    {
        using baseClass = Trap;

    public:

        //! Constructor.
        PickPock(const sf::Vector2u& coords, TrapData& trapdata);

        //! Default destructor.
        ~PickPock() = default;

        //------------------//
        //! @name Resources
        //! @{

        void setDosh(uint32 value);
        inline uint32 dosh() const { return m_trapdata[L"dosh"].as_uint32(); }
        inline void addDosh(uint32 amount) { setDosh(dosh() + amount); }
        inline void subDosh(uint32 amount) { setDosh(dosh() - amount); }

        inline uint32 harvestDosh() final { auto harvestedDosh = harvestableDosh(); subDosh(harvestedDosh); return harvestedDosh; }
        inline uint32 harvestableDosh() const final { return dosh(); }

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

        void receive(const Event& event) final;

        //! @}

        //-----------------//
        //! @name Stealing
        //! @{

        void stealFromHero(Hero* hero);

        //! @}

    private:

        uint m_maxDosh = 20u;           //!< The max amount of dosh this trap can hold.

        // Decorum
        sf::RectangleShape m_sprite;    //!< The sprite.
    };
}
}
