#pragma once

#include "context/context.hpp"

namespace nui
{

    template<typename Value_t>
    SpinBox<Value_t>::SpinBox()
        : m_minLimit(0)
        , m_maxLimit(-1)
    {
        // Entry
        attachChild(m_entry);
        m_entry.setLength(7u);

        // Plus/minus
        addPart(&m_plus);
        addPart(&m_minus);
        m_plus.setTexture(&context::context.textures.get("core/nui/plus"));
        m_minus.setTexture(&context::context.textures.get("core/nui/minus"));
    }

    //-------------------//
    //----- Routine -----//

    template<typename Value_t>
    void SpinBox<Value_t>::updateRoutine(const sf::Time& dt)
    {
        // Click repeat
        if (m_crActive) {
            m_crTimer -= dt.asSeconds();
            if (m_crTimer <= 0.f) {
                m_crTimer = m_crDelayBetweenRepeats;
                doActionFromStoredPosition();
            }
        }
    }

    template<typename Value_t>
    void SpinBox<Value_t>::refreshNUI(const config::NUIGuides& cNUI)
    {
        baseClass::refreshNUI(cNUI);
        refreshEntry();
    }

    //------------------//
    //----- Events -----//

    template<typename Value_t>
    bool SpinBox<Value_t>::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f&)
    {
        returnif (button != sf::Mouse::Left) false;

        // Activate click repeat
        m_crActive = true;
        m_crTimer = m_crDelayBeforeRepeating;
        m_storedPosition = mousePos;

        doActionFromStoredPosition();
        return true;
    }

    template<typename Value_t>
    bool SpinBox<Value_t>::handleMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f&, const sf::Vector2f&)
    {
        returnif (button != sf::Mouse::Left) false;

        // Deactivate click repeat
        m_crActive = false;
        return true;
    }

    template<typename Value_t>
    void SpinBox<Value_t>::handleMouseLeft()
    {
        // Deactivate click repeat
        m_crActive = false;
    }

    //--------------------//
    //----- Callback -----//

    template<typename Value_t>
    void SpinBox<Value_t>::doActionFromStoredPosition()
    {
        // Check if a +/- is clicked
        returnif (m_storedPosition.x < m_entry.size().x);

        if (m_storedPosition.y < m_entry.size().y / 2.f) addStep();
        else subStep();
    }

    template<typename Value_t>
    void SpinBox<Value_t>::setOnValueChangeCallback(const ValueChangeCallback& callback)
    {
        m_callback = callback;

        m_entry.setOnValueChangeCallback([this] (Value_t oldValue, Value_t value) {
            // Readjust to limit
            if (value < m_minLimit)      m_value = m_minLimit;
            else if (value > m_maxLimit) m_value = m_maxLimit;
            else m_value = value;

            // Call callback if exists
            m_entry.setValue(m_value, false); // Will not send a callback
            if (m_callback != nullptr)
                m_callback(oldValue, m_value);
        });
    }

    template<typename Value_t>
    void SpinBox<Value_t>::setOnValidateCallback(const ValidateCallback& validateCallback)
    {
        m_entry.setOnValidateCallback(validateCallback);
    }

    //----------------------------//
    //----- Value management -----//

    template<typename Value_t>
    void SpinBox<Value_t>::set(Value_t value)
    {
        m_value = value;
        refreshEntry();
    }

    template<typename Value_t>
    void SpinBox<Value_t>::addStep()
    {
        m_value += m_step;
        if (m_value < m_minLimit || m_value > m_maxLimit)
            m_value = m_maxLimit;

        refreshEntry();
    }
    template<typename Value_t>
    void SpinBox<Value_t>::subStep()
    {
        m_value -= m_step;
        if (m_value < m_minLimit || m_value > m_maxLimit)
            m_value = m_minLimit;

        refreshEntry();
    }

    //-----------------------------------//
    //----- Internal changes update -----//

    template<typename Value_t>
    void SpinBox<Value_t>::refreshEntry()
    {
        // Send a callback
        m_entry.setValue(m_value, true);
        const auto& entrySize = m_entry.size();

        // Re-position plus/minus
        m_plus.setPosition({entrySize.x, 0.f});
        m_minus.setPosition(entrySize);

        auto signSize = entrySize.y / 2.f;
        m_plus.setSize({signSize, signSize});
        m_minus.setSize({signSize, signSize});
        m_minus.setOrigin({0.f, signSize});

        setSize(entrySize + sf::Vector2f{m_plus.getSize().x, 0.f});
    }
}
