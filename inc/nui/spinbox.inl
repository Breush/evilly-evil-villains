#pragma once

#include "core/application.hpp"
#include "resources/identifiers.hpp"

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
        m_plus.setTexture(&Application::context().textures.get(TextureID::NUI_PLUS));
        m_minus.setTexture(&Application::context().textures.get(TextureID::NUI_MINUS));

        refreshDisplay();
    }

    //-------------------//
    //----- Routine -----//

    template<typename Value_t>
    void SpinBox<Value_t>::refreshDisplay()
    {
        baseClass::refreshDisplay();
        refreshEntry();
    }

    //------------------//
    //----- Events -----//

    template<typename Value_t>
    void SpinBox<Value_t>::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f&)
    {
        returnif (button != sf::Mouse::Left);

        // Check if a +/- is clicked.
        returnif (mousePos.x < m_entry.size().x);

        if (mousePos.y < m_entry.size().y / 2.f) addStep();
        else subStep();
    }

    //--------------------//
    //----- Callback -----//

    template<typename Value_t>
    void SpinBox<Value_t>::setCallback(const Callback& callback)
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
