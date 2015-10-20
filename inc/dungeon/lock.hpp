#pragma once

#include "scene/entity.hpp"
#include "context/event.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace dungeon
{
    // Forward declarations

    class Data;

    //! A button-like lock which controls the invasion/design state of the dungeon.

    class Lock final : public scene::Entity, private context::EventReceiver
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        Lock();

        //! Default destructor.
        ~Lock() = default;

        std::string _name() const final { return "dungeon::Lock"; }

        //! Initialize after textures are loaded.
        void init();

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

        void onSizeChanges() final;
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! @}

        //----------------//
        //! @name Events
        //! @{

        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        void receive(const context::Event& event) final;

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Refresh the display of the lock given the current mode.
        void updateMode();

        //! Refresh sprite scale using current size.
        void refreshImageScale();

        //! @}

    private:

        Data* m_data = nullptr; //!< Dungeon data.

        sf::Sprite m_lock;          //!< Display sprite for design/invasion modes.
        sf::Vector2f m_targetSize;  //!< The visible size of the lock.
    };
}
