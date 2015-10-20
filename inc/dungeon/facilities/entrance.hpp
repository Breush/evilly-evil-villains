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

        std::string _name() const final { return "dungeon::facilities::Entrance"; }

    protected:

        //---------------//
        //! @name Events
        //! @{

        void receive(const context::Event& event) final {}

        //! @}

    private:

        scene::AnimatedSprite m_sprite;   //!< The sprite.
    };
}
}
