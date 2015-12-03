#pragma once

#include "nui/entity.hpp"
#include "sfe/line.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

namespace nui
{
    //! An interactive entry allowing user to type text.

    class TextEntry : public nui::Entity
    {
        using baseClass = nui::Entity;

        //! Callback type used when a change occurs.
        using TextChangeCallback = std::function<void(const std::wstring&, const std::wstring&)>;

        //! Callback type used when user press Return.
        using ValidateCallback = std::function<void()>;

    public:

        //! Constructor.
        TextEntry();

        //! Default destructor.
        virtual ~TextEntry() = default;

        virtual std::string _name() const override { return "nui::TextEntry"; }

        //--------------------------//
        //! @name Getter and setter
        //! @{

        //! The current string.
        inline std::wstring text() const { return m_textString.toWideString(); };

        //! The current string before the cursor position.
        inline std::wstring textBeforeCursor() const { return m_cursorString.toWideString(); };

        //! The current string after the cursor position.
        inline std::wstring textAfterCursor() const { return m_textString.substring(m_cursorString.getSize()); };

        //! Set the cursor to an absolute position.
        void setCursorPosition(uint position);

        //! Set the current string and update, if sendCallback is false a callback will not be sent.
        void setText(const std::wstring& str, bool sendCallback = true);

        //! Sets the callback called whenever the text changes.
        void setOnTextChangeCallback(const TextChangeCallback& callback);

        //! Sets the callback called whenever the user presses Return.
        void setOnValidateCallback(const ValidateCallback& callback);

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! Size override, control only the width.
        PARAMGSU(float, m_width, width, setWidth, updateSize)

        //! Size override, length is an estimated number of characters visible.
        PARAMGSU(uint, m_length, length, setLength, updateSize)

        //! The max number of characters.
        PARAMGS(uint, m_maxCharacters, maxCharacters, setMaxCharacters)

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void updateRoutine(const sf::Time& dt) final;
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! Recompute the size of the TextEntry.
        void updateSize();

        //! @}

        //---------------//
        //! @name Events
        //! @{

        bool handleKeyboardEvent(const sf::Event& event) override;
        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        //! @}

        //---------------//
        //! @name Delete
        //! @{

        //! Remove the current selection.
        bool deleteSelection(bool startRefresh = true);

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

        //! Update the visual text from the current string.
        void refreshText(bool sendCallback);

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

        TextChangeCallback m_onTextChangeCallback;  //!< Whenever the value changes.
        ValidateCallback m_onValidateCallback;      //!< Whenever user presses Return.
    };
}
