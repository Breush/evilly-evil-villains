#pragma once

#include "dungeon/facilities.hpp"
#include "scene/wrappers/animatedsprite.hpp"

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
        Entrance(const sf::Vector2u& coords, ElementData& elementdata, dungeon::Inter& inter);

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
