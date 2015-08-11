#pragma once

#include "dungeon/monsters.hpp"
// #include "sfe/animatedsprite.hpp"
#include "sfe/rectangleshape.hpp"

namespace dungeon
{
namespace monsters
{
    //!< A creepim explodes when there are nearby heroes.

    class Creepim final : public Monster
    {
        using baseClass = Monster;

    public:

        //! Constructor.
        Creepim(const sf::Vector2u& coords, ElementData& elementdata);

        //! Default destructor.
        ~Creepim() = default;

    protected:

        //---------------//
        //! @name Events
        //! @{

        void receive(const Event& event) final;

        //! @}

    private:

        // TODO sfe::AnimatedSprite m_sprite;   //!< The sprite.
        sfe::RectangleShape m_sprite;
    };
}
}
