#pragma once

#include "scene/entity.hpp"
#include "nui/numberentry.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

#include <functional>

namespace nui
{
    //! A basic tool to let the user edit numbers.

    template<typename Value_t>
    class SpinBox final : public scene::Entity
    {
        using baseClass = scene::Entity;

        //! Callback type used when a change occurs.
        using Callback = std::function<void(Value_t, Value_t)>;

    public:

        //! Constructor.
        SpinBox();

        //! Default destructor.
        ~SpinBox() = default;

        //-------------------------//
        //! @name Value management
        //! @{

        //! Access to the entry.
        nui::NumberEntry<Value_t>& entry() { return m_entry; }

        //! Sets the value to the specified one.
        //! This does not check against limits and does not send a callback.
        void set(Value_t value);

        //! Sets the value of the step to add on +/-.
        void setStep(Value_t step) { m_step = step; }

        //! Called when clicking on +.
        void addStep();

        //! Called when clicking on -.
        void subStep();

        //! @}

        //-----------------//
        //! @name Callback
        //! @{

        //! Sets the callback to the specified one.
        void setCallback(const Callback& callback);

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //!< The min limit for the number.
        PARAMGS(Value_t, m_minLimit, minLimit, setMinLimit);

        //!< The max limit for the number.
        PARAMGS(Value_t, m_maxLimit, maxLimit, setMaxLimit);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void updateRoutine(const sf::Time& dt) final;
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        void handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        //! @}

        //-----------------//
        //! @name Callback
        //! @{

        //! Add or sub given stored position.
        void doActionFromStoredPosition();

        //! @}

        //--------------------------------//
        //! @name Internal changes update
        //! @{

        //! Called to refresh the value printed in the entry.
        void refreshEntry();

        //! @}

    private:

        Value_t m_value;    //!< The current and displayed value.
        Value_t m_step;     //!< The value to add or substract when +/-.

        Callback m_callback = nullptr;  //!< The callback function called on any change.
        sf::Vector2f m_storedPosition;  //!< Stored position of last mouse left click.

        // NUI
        nui::NumberEntry<Value_t> m_entry;   //!< The entry itself.
        sf::RectangleShape m_plus;  //!< The + sign.
        sf::RectangleShape m_minus; //!< The - sign.

        // Click repeat
        bool m_crActive = false;                        //!< Whether or not the click repeat is active.
        float m_crTimer = -1.f;                         //!< Set timer, or virtual click will be sent when negative.
        const float m_crDelayBeforeRepeating = 0.6f;    //!< Initial delay before enabling repeating.
        const float m_crDelayBetweenRepeats = 0.1f;     //!< Delay between two repeats.
    };
}

#include "nui/spinbox.inl"
