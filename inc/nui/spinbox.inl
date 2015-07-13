#pragma once
#include "nui/spinbox.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"

namespace nui
{

    template<typename Value_t>
    SpinBox<Value_t>::SpinBox()
    {
        // Label
        attachChild(m_label);
        m_label.setPrestyle(sfe::Label::Prestyle::NUI);

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
    }

    //------------------//
    //----- Events -----//

    template<typename Value_t>
    void SpinBox<Value_t>::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f&)
    {
        returnif (button != sf::Mouse::Left);

        // Check if a +/- is clicked.
        returnif (mousePos.x < m_label.size().x);

        if (mousePos.y < m_label.size().y / 2.f) addStep();
        else subStep();
    }

    //----------------------------//
    //----- Value management -----//

    template<typename Value_t>
    void SpinBox<Value_t>::set(Value_t value)
    {
        m_value = value;
        refreshLabel();
    }

    template<typename Value_t>
    void SpinBox<Value_t>::addStep()
    {
        auto oldValue = m_value;
        m_value += m_step;

        refreshLabel();
        if (m_callback != nullptr)
            m_callback(oldValue, m_value);
    }
    template<typename Value_t>
    void SpinBox<Value_t>::subStep()
    {
        auto oldValue = m_value;
        m_value -= m_step;

        refreshLabel();
        if (m_callback != nullptr)
            m_callback(oldValue, m_value);
    }

    //-----------------------------------//
    //----- Internal changes update -----//

    template<typename Value_t>
    void SpinBox<Value_t>::refreshLabel()
    {
        std::wstringstream str;
        str << m_prefix << m_value << m_postfix;
        m_label.setText(str.str());
        const auto& labelSize = m_label.size();

        // Re-position plus/minus
        m_plus.setPosition({labelSize.x, 0.f});
        m_minus.setPosition(labelSize);

        auto signSize = labelSize.y / 2.f;
        m_plus.setSize({signSize, signSize});
        m_minus.setSize({signSize, signSize});
        m_minus.setOrigin({0.f, signSize});

        setSize(labelSize + sf::Vector2f{m_plus.getSize().x, 0.f});
    }
}
