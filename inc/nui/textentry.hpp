#pragma once

#include "scene/entity.hpp"
#include "sfe/line.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

namespace nui
{
    class TextEntry final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:
        TextEntry();
        virtual ~TextEntry() = default;

        // Routine
        void update() final;
        void updateSize();
        void updateRoutine(const sf::Time& dt) final;
        void refreshDisplay() final;

        // Getter
        std::wstring text() const { return m_textString.toWideString(); };

        // Size override, length is an estimated number of characters to draw.
        // To limit the max number of characters, use setMaxCharacters().
        PARAMGSU(uint, m_length, length, setLength, updateSize)
        PARAMGS(uint, m_maxCharacters, maxCharacters, setMaxCharacters)

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
        float m_fontVSpace;
        float m_fontHSpace;
    };
}
