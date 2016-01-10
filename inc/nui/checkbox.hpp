#pragma once

#include "nui/entity.hpp"
#include "tools/int.hpp"

namespace nui
{
    //! An on/off box.

    class CheckBox final : public Entity
    {
        using baseClass = Entity;

    public:

        //! Constructor.
        CheckBox();

        //! Default destructor.
        virtual ~CheckBox() = default;

        virtual std::string _name() const override { return "nui::ChoiceBox"; }

        //---------------//
        //! @name Status
        //! @{

        //! Set the box's status to true or false.
        void setStatus(bool status);

        //! Get the box's status.
        inline bool status() const { return m_status; }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        bool handleKeyboardEvent(const sf::Event& event) final;
        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Update the sprite to the current status.
        void updateSprite();

        //! @}

    private:

        bool m_status = false;  //!< The box's status (on or off).

        sf::RectangleShape m_sprite;    //!< The button's sprite.
    };
}
