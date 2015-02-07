#pragma once

#include "nui/object.hpp"
#include "tools/int.hpp"
#include "sfe/line.hpp"
#include "sfe/arrow.hpp"

#include <SFML/Graphics/Text.hpp>

#include <vector>
#include <string>
#include <functional>

namespace nui
{
    class ChoiceBox : public Object
    {
        typedef Object baseClass;

    public:
        typedef std::function<void()> Callback;

        ChoiceBox();
        virtual ~ChoiceBox() {}

        // Virtual
        virtual void init() override;
        virtual void update() override;
        bool handleKeyboardEvent(const sf::Event& event) override;
        //bool handleJoystickEvent(const sf::Event& event) override;

        // Choices
        void add(const std::wstring& text, const Callback callback = nullptr);
        void acceptChoice();
        void switchChoiceLeft();
        void switchChoiceRight();
        void setChoice(uint choice);
        void setChoiceText(uint choice, const std::wstring& text);
        void setChoiceCallback(uint choice, const Callback callback);

    protected:
        // Mouse events
        virtual void handleMouseButtonPressed(const sf::Mouse::Button& mouseButton, const sf::Vector2f& mousePos) override;
        virtual void handleMouseMoved(const sf::Vector2f& mousePos) override;
        virtual void handleMouseLeft() override;

        bool isLeftArrowSelected(const float& x);
        bool isRightArrowSelected(const float& x);

        // Updates
        void updateButtonSize();
        virtual void updateSize();

        // Params
        PARAMGSU(bool, m_showArrows, showArrows, setShowArrows, updateSize)
        PARAMGSU(bool, m_showLines, showLines, setShowLines, updateSize)
        PARAMG(sf::Vector2f, m_buttonSize, buttonSize)
        PARAMG(float, m_arrowOffset, arrowOffset)
        PARAMG(float, m_lineOffset, lineOffset)
        PARAMG(float, m_arrowSize, arrowSize)
        PARAMG(float, m_lineSize, lineSize)
        PARAMG(uint, m_choice, choice)

        // Keep infos
        struct ChoiceInfo {
            std::wstring text;
            Callback callback;
        };

        // Getters for children
        sf::Text& text() { return m_text; }
        sfe::HLine& topLine() { return m_topLine; }
        sfe::HLine& botLine() { return m_botLine; }
        const sf::Vector2f& maxTextSize() const { return m_maxTextSize; }

    private:
        // Choices
        std::vector<ChoiceInfo> m_choices;
        uint m_nChoices;

        // Text
        sf::Text m_text;
        sf::Vector2f m_textSize;
        sf::Vector2f m_maxTextSize;

        // Lines
        sfe::HLine m_topLine;
        sfe::HLine m_botLine;

        // Arrows
        sfe::LArrow m_lArrow;
        sfe::RArrow m_rArrow;
    };
}
