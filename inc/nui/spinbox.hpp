#pragma once

#include "scene/entity.hpp"
#include "sfe/label.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

#include <functional>

namespace nui
{
    //! A basic tool to let the user edit numbers.
    // TODO Edit the number itself with a NumberEntry (~ TextEntry)
    // TODO Add limits
    // TODO Enable long-click repeat
    // TODO Add padding

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

        //----------------//
        //! @name Display
        //! @{

        //! Sets the prefix shown before the value.
        void setPrefix(std::wstring prefix) { m_prefix = std::move(prefix); }

        //! Sets the postfix shown after the value.
        void setPostfix(std::wstring postfix) { m_postfix = std::move(postfix); }

        //! @}

        //-----------------//
        //! @name Callback
        //! @{

        //! Sets the callback to the specified one.
        void setCallback(const Callback& callback) { m_callback = callback; }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void refreshDisplay() final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        void handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;

        //! @}

        //--------------------------------//
        //! @name Internal changes update
        //! @{

        //! Called to refresh the value printed in the label.
        void refreshLabel();

        //! @}

    private:

        Value_t m_value;    //!< The current and displayed value.
        Value_t m_step;     //!< The value to add or substract when +/-.
        std::wstring m_prefix;  //!< The prefix.
        std::wstring m_postfix; //!< The postfix.

        Callback m_callback = nullptr;  //!< The callback function called on any change.

        // NUI
        sfe::Label m_label;         //!< The label itself.
        sf::RectangleShape m_plus;  //!< The + sign.
        sf::RectangleShape m_minus; //!< The - sign.
    };
}

#include "nui/spinbox.inl"
