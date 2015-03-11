#pragma once

#include "scene/entity.hpp"
#include "tools/int.hpp"
#include "sfe/line.hpp"
#include "sfe/arrow.hpp"

#include <SFML/Graphics/Text.hpp>

#include <vector>
#include <string>
#include <functional>

namespace nui
{
    class ChoiceBox : public scene::Entity
    {
        using baseClass = scene::Entity;

    protected:
        using Callback = std::function<void()>;

    public:
        ChoiceBox();
        virtual ~ChoiceBox() = default;

        // Routine
        void updateRoutine(const sf::Time& dt) override;

        // Choices
        void add(const std::wstring& text, const Callback callback = nullptr);
        void acceptChoice();
        void switchChoiceLeft();
        void switchChoiceRight();
        void setChoice(uint choice);
        void setChoiceText(uint choice, const std::wstring& text);
        void setChoiceCallback(uint choice, const Callback callback);

    protected:
        // Virtual
        virtual void update() override;

        // Events
        void handleKeyboardEvent(const sf::Event& event) override;
        virtual void handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        virtual void handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        virtual void handleMouseLeft() override;

        bool isLeftArrowSelected(const float& x);
        bool isRightArrowSelected(const float& x);

        // Updates
        void updateButtonSize();
        virtual void updateSize();

        // Params
        PARAMGSU(bool, m_showArrows, showArrows, setShowArrows, updateSize)
        PARAMGSU(bool, m_showLines, showLines, setShowLines, updateSize)
        PARAMG(bool, m_choiceChanged, choiceChanged)
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
        const sf::Vector2f& buttonSize() const { return m_buttonSize; }

    private:
        // Choices
        std::vector<ChoiceInfo> m_choices;
        uint m_nChoices = 0u;

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

        // Properties
        sf::Vector2f m_buttonSize;
        float m_arrowOffset;
        float m_lineOffset;
        float m_arrowSize;
        float m_lineSize;
    };
}
