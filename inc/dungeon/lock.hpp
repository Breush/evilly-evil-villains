#pragma once

#include "scene/entity.hpp"
#include "dungeon/event.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace dungeon
{
    // Forward declarations

    class Data;

    //! A button-like lock which controls the invasion/design state of the dungeon.

    class Lock final : public scene::Entity, private EventReceiver
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        Lock();

        //! Default destructor.
        ~Lock() = default;

        //------------------------//
        //! @name Mode management
        //! @{

        //! Tries to change the current mode to the other one.
        void switchMode();

        //! @}

        //--------------------//
        //! @name Data events
        //! @{

        //! Set the dungeon data source.
        void useData(Data& data);

        //! @}

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

        void receive(const Event& event) final;

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Refresh the display of the lock given the current mode.
        void updateMode();

        //! @}

    private:

        Data* m_data = nullptr; //!< Dungeon data.

        sf::Sprite m_lock;          //!< Display Sprite for design/invasion modes.
        sf::Vector2f m_targetSize;  //!< The visible size of the lock.
    };
}
