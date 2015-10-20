#pragma once

#include "dungeon/facilities.hpp"
#include "scene/wrappers/sprite.hpp"

namespace dungeon
{
namespace facilities
{
    //!< The ladder facility.

    class Ladder final : public Facility
    {
        using baseClass = Facility;

    public:

        //! Defines in what mode the ladder is drawn.
        enum class Design
        {
            MAIN,
            EXIT_END,
            EXIT_MAIN,
        };

    public:

        //! Constructor.
        Ladder(const sf::Vector2u& coords, ElementData& elementdata, dungeon::Inter& inter);

        //! Default destructor.
        ~Ladder() = default;

        std::string _name() const final { return "dungeon::facilities::Ladder"; }

        //---------------//
        //! @name Design
        //! @{

        //! Defines in what mode the ladder is drawn.
        void setDesign(Design design);

        //! @}

    protected:

        //---------------//
        //! @name Events
        //! @{

        void receive(const context::Event& event) final {}

        //! @}

    private:

        scene::Sprite m_sprite;   //!< The sprite.
    };
}
}
