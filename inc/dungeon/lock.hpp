#pragma once

#include "scene/entity.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace dungeon
{
    //! A button-like lock which controls the invasion/design state of the dungeon.

    class Lock final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        Lock();

        //! Default destructor.
        ~Lock() = default;

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void update() final;
        void refreshDisplay() final;

        //! @}

        //----------------//
        //! @name Events
        //! @{

        void handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        //! @}

    private:

        sf::RectangleShape m_lock;  //!< Switch design/invasion modes.

        // Decorum

        sf::Vector2f m_targetSize;  //!< The visible size of the lock.
    };
}
