#pragma once

#include "scene/wrappers/animatedsprite.hpp"
#include "scene/wrappers/richlabel.hpp"

// Forward declarations

namespace states
{
    class State;
}

namespace hub
{
    class Selector final : public scene::Entity
    {
    public:

        //! Constructor;
        Selector(states::State& state);

        //! Default destructor;
        ~Selector() = default;

        std::string _name() const final { return "hub::Selector"; }

        //! Initialize the object.
        void init();

    protected:

        //---------------//
        //! @name Events
        //! @{

        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Select the correct animation over this position.
        void refreshHitboxSelected(const sf::Vector2f& relPos);

        //! @}

    private:

        states::State& m_state;             //!< The original state.
        std::string m_selectedBox = "none"; //!< The selected box.

        // Decorum
        scene::AnimatedSprite m_sprite;     //!< Basically, everything.
        scene::RichLabel m_selectedLabel;   //!< The label explaining what's hovered.
    };
}
