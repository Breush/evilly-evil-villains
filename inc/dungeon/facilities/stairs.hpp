#pragma once

#include "dungeon/facilities.hpp"
#include "scene/wrappers/animatedsprite.hpp"

namespace dungeon
{
namespace facilities
{
    //!< The stairs facility.

    class Stairs final : public Facility
    {
        using baseClass = Facility;

    public:

        //! Defines in what mode the stairs is drawn.
        enum class Design
        {
            UP_LEFT,
            UP_RIGHT,
            DOWN_LEFT,
            DOWN_RIGHT,
        };

    public:

        //! Constructor.
        Stairs(const sf::Vector2u& coords, ElementData& elementdata, dungeon::Inter& inter);

        //! Default destructor.
        ~Stairs() = default;

        std::string _name() const final { return "dungeon::facilities::Stairs"; }

        //---------------//
        //! @name Design
        //! @{

        //! Defines in what mode the stairs is drawn.
        void setDesign(Design design);

        //! @}

    protected:

        //---------------//
        //! @name Events
        //! @{

        void receive(const context::Event& event) final {}

        //! @}

    private:

        scene::AnimatedSprite m_sprite; //!< The sprite.
    };
}
}
