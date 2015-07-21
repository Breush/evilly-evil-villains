#pragma once

#include "dungeon/facilities.hpp"
#include "sfe/animatedsprite.hpp"

namespace dungeon
{
namespace facilities
{
    //!< A treasure chest.

    class Treasure final : public Facility
    {
        using baseClass = Facility;

    public:

        //! Constructor.
        Treasure(const sf::Vector2u& coords, ElementData& elementdata);

        //! Default destructor.
        ~Treasure() = default;

        //------------------//
        //! @name Resources
        //! @{

        //! Set the dosh.
        void setDosh(uint32 value);

        //! Get the dosh.
        inline uint32 dosh() const { return m_elementdata[L"dosh"].as_uint32(); }

        //! @}

    protected:

        //---------------//
        //! @name Events
        //! @{

        void receive(const Event& event) final {}

        //! @}

    private:

        sfe::AnimatedSprite m_sprite;   //!< The sprite.
    };
}
}
