#pragma once

#include "scene/entity.hpp"
#include "sfe/line.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

namespace nui
{
    //! An interactive entry allowing user to type text.

    class TextEntry final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        TextEntry();

        //! Default destructor.
        ~TextEntry() = default;

        //--------------------------//
        //! @name Getter and setter
        //! @{

        //! The current string.
        std::wstring text() const { return m_textString.toWideString(); };

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        // Size override, length is an estimated number of characters visible.
        PARAMGSU(uint, m_length, length, setLength, updateSize)

        // The max number of characters.
        PARAMGS(uint, m_maxCharacters, maxCharacters, setMaxCharacters)

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void updateRoutine(const sf::Time& dt) final;
        void refreshDisplay() final;

        //! Recompute the size of the TextEntry.
        void updateSize();

        //! @}

        //---------------//
        //! @name Events
        //! @{

        void handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleKeyboardEvent(const sf::Event& event) final;

        //! @}

        //---------------//
        //! @name Delete
        //! @{

        //! Remove the current selection.
        bool deleteSelection();

        //! Remove the previous word/character.
        void deletePrevious();

        //! Remove the next word/character.
        void deleteNext();

        //! @}

        //------------------//
        //! @name Selection
        //! @{

        //! Select the text between start and end.
        //! The limits are not checked and should exists.
        void select(uint start, uint end);

        //! Select the whole text.
        void selectAll();

        //! Cancel selection.
        bool deselect();

        //! @}

        //---------------//
        //! @name Cursor
        //! @{

        //! Returns the character position from relX, text will be filled with the string before this point.
        uint getCharacterPosition(const float relX, sf::Text& text);

        //! Add a character to the string where the cursor is.
        void addCharacter(const uint32_t character);

        //! Moves the cursor of some characters.
        void moveCursor(int relativePos);

        //! Returns the start position of the previous word relative to cursor.
        int previousRelativeWord();

        //! Returns the start position of the next word relative to cursor.
        int nextRelativeWord();

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Refresh overlay of selection from m_selectString.
        void refreshSelection();

        //! Refresh overlay position of selection.
        void refreshSelectionPosition();

        //! Refresh the text position and move it if cursor is too far.
        void updateDynamicText();

        //! @}

    private:

        sf::RectangleShape m_background;    //< The background.

        // Text
        sf::String m_textString;        //!< The effective current string.
        sf::Text m_text;                //!< The drawable text.
        float m_textShift = 0.f;        //!< Applied to follow the cursor when needed.
        float m_textWidthLimit = 0.f;   //!< The max visible width for the text.

        // Cursor
        bool m_cursorOn = true;         //!< Whether the cursor should blink.
        sfe::VLine m_cursor;            //!< The drawable cursor.
        float m_cursorBlinkTime = 0.f;  //!< How many seconds the cursor has been shown.
        sf::String m_cursorString;      //!< The text before the cursor.
        sf::Text m_cursorText;          //!< The text before the cursor.

        // Selection
        bool m_selecting = false;           //!< Whether the user has pressed the mouse button.
        uint m_selectStart = 0u;            //!< The offset of start of selection.
        sf::String m_selectString;          //!< The text selected.
        sf::Text m_selectText;              //!< The text selected.
        sf::RectangleShape m_selectRect;    //!< The overlay for selection
        float m_selectShiftStart = 0.f;     //!< The shift from start of selection.
        bool m_selectL2R = true;            //!< True if selection is left to right from start.

        // Decorum
        float m_textPadding = 0.f;  //!< The extra space between text and border.
        float m_fontVSpace = 0.f;   //!< The estimated vertical space reserved for a character.
        float m_fontHSpace = 0.f;   //!< The estimated horizontal space reserved for a character.
    };
}
