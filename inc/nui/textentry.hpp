#pragma once

#include "scene/entity.hpp"
#include "sfe/line.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

namespace nui
{
    class TextEntry : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:
        TextEntry();
        virtual ~TextEntry() = default;

        // Routine
        void update() override;
        void updateRoutine(const sf::Time& dt) override;

        // Size override
        void setLength(float length);

        // Getter
        std::wstring text() const { return m_textString.toWideString(); };

    protected:
        // Events
        bool handleKeyboardEvent(const sf::Event& event) override;

        // Dynamics of text and cursor
        void moveCursor(int relativePos);
        void updateDynamicText();
        int previousRelativeWord();
        int nextRelativeWord();

    private:
        // Background
        sf::RectangleShape m_background;

        // Text
        sf::String m_textString;
        sf::Text m_text;

        // Cursor
        bool m_cursorOn = true;
        sfe::VLine m_cursor;
        float m_cursorBlinkTime = 0.f;
        sf::String m_cursorString;
        sf::Text m_cursorText;

        // Decorum
        float m_textPadding;
    };
}
