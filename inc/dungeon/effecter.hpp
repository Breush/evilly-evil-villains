#pragma once

#include "scene/wrappers/animatedsprite.hpp"
#include "dungeon/structs/room.hpp"

#include <unordered_map>
#include <functional>
#include <memory>
#include <deque>

namespace dungeon
{
    // Forward declarations

    class Inter;

    //! Controls effects played over the dungeon::Inter.
    // TODO This should probably use a pool, as this is almost like a particules system.

    class Effecter final
    {
    public:

        //! Constructor.
        Effecter();

        //! Default destructor.
        ~Effecter() = default;

        //! Initialize the memory.
        void init();

        //----------------//
        //! @name Routine
        //! @{

        //! Update the internal state.
        void update(const sf::Time& dt);

        //! @}

        //----------------//
        //! @name Binding
        //! @{

        //! Binds inter to the effecter.
        void useInter(Inter& inter);

        //! @}

        //------------------------//
        //! @name Effects control
        //! @{

        //! Adds a new effect at a specific room.
        void add(const std::string& key, const RoomCoords& coords);

        //! @}

    protected:

        //------------------//
        //! @name Factories
        //! @{

        //! Register a new key -> animation in the factory.
        void addFactory(const std::string& key, const std::string& animationID);

        //! @}

    private:

        Inter* m_inter = nullptr;   //!< Binded inter.

        // Factories
        using Effect = scene::AnimatedSprite;
        using FactoryElement = std::function<std::unique_ptr<Effect>()>;
        std::unordered_map<std::string, FactoryElement> m_factories;    //!< Creates a new AnimatedSprite as registered.

        // Effects
        std::deque<std::unique_ptr<Effect>> m_effects;  //!< All the current effects.
    };
}
