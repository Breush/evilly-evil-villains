#pragma once

#include "dungeon/facilities.hpp"
#include "sfe/animatedsprite.hpp"

namespace dungeon
{
namespace facilities
{
    //!< The entrance to the dungeon.

    class Entrance final : public Facility
    {
        using baseClass = Facility;

    public:

        //! Constructor.
        Entrance(const sf::Vector2u& coords, ElementData& elementdata);

        //! Default destructor.
        ~Entrance() = default;

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