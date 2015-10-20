#pragma once

#include "dungeon/facilities.hpp"
#include "scene/wrappers/animatedsprite.hpp"

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
        Treasure(const sf::Vector2u& coords, ElementData& elementdata, dungeon::Inter& inter);

        //! Default destructor.
        ~Treasure() = default;

        std::string _name() const final { return "dungeon::facilities::Treasure"; }

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

        void receive(const context::Event& event) final {}

        //! @}

    private:

        scene::AnimatedSprite m_sprite;   //!< The sprite.
    };
}
}
