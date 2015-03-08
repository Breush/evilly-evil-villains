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
        void update();
        void updateRoutine(const sf::Time& dt);

        // Size override
        void setLength(float length);

    protected:
        // Events
        void handleKeyboardEvent(const sf::Event& event) override;

        // Dynamics of text and cursor
        void moveCursor(int relativePos);
        void updateDynamicText();
        int previousRelativeSpace();
        int nextRelativeSpace();

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
