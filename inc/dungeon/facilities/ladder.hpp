#pragma once

#include "dungeon/facilities.hpp"
#include "sfe/sprite.hpp"

namespace dungeon
{
namespace facilities
{
    //!< The ladder facility.

    class Ladder final : public Facility
    {
        using baseClass = Facility;

    public:

        //! Constructor.
        Ladder(const sf::Vector2u& coords, ElementData& elementdata);

        //! Default destructor.
        ~Ladder() = default;

    protected:

        //---------------//
        //! @name Events
        //! @{

        void receive(const Event& event) final {}

        //! @}

    private:

        sfe::Sprite m_sprite;   //!< The sprite.
    };
}
}
