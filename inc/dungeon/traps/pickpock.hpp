#pragma once

#include "dungeon/traps.hpp"
#include "scene/wrappers/animatedsprite.hpp"

namespace dungeon
{
    // Forward declarations

    class Hero;

namespace traps
{
    //!< A pick-pock steals money from nearby heroes.

    class PickPock final : public Trap
    {
        using baseClass = Trap;

    public:

        //! Constructor.
        PickPock(const sf::Vector2u& coords, ElementData& elementdata, dungeon::Inter& inter);

        //! Default destructor.
        ~PickPock() = default;

        //------------------//
        //! @name Resources
        //! @{

        //! Set the dosh.
        void setDosh(uint32 value);

        //! Get the dosh.
        inline uint32 dosh() const { return m_elementdata[L"dosh"].as_uint32(); }

        //! Add to dosh.
        inline void addDosh(uint32 amount) { setDosh(dosh() + amount); }

        //! Sub from dosh.
        inline void subDosh(uint32 amount) { setDosh(dosh() - amount); }

        //! Harvest the dosh.
        inline uint32 harvestDosh() final { auto harvestedDosh = harvestableDosh(); subDosh(harvestedDosh); return harvestedDosh; }

        //! Get how much dosh is harvestable from here.
        inline uint32 harvestableDosh() const final { return dosh(); }

        //! @}

    protected:

        //---------------//
        //! @name Events
        //! @{

        void receive(const context::Event& event) final;

        //! @}

        //-----------------//
        //! @name Stealing
        //! @{

        //! Steal maxDosh from hero if possible.
        void stealFromHero(Hero* hero);

        //! @}

    private:

        uint m_maxDosh = 20u;           //!< The max amount of dosh this trap can hold.

        scene::AnimatedSprite m_sprite;   //!< The sprite.
    };
}
}
