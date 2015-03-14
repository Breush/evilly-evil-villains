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
    //! A multi-choices interactive box.
    /*!
     *  This multi-choices button makes a excellent
     *  UI object to let the user choose between
     *  a few options.
     */

    class ChoiceBox : public scene::Entity
    {
        using baseClass = scene::Entity;

    protected:

        //! Type for function callback when button is clicked.
        using Callback = std::function<void()>;

        //! A choice is a wide string and a function callback.
        struct ChoiceInfo
        {
            std::wstring text;  //!< The text to display when choice is selected.
            Callback callback;  //!< The callback function to call when choice accepted.
        };

    public:

        //! Constructor.
        ChoiceBox();

        //! Default destructor.
        virtual ~ChoiceBox() = default;

        //----------------//
        //! @name Choices
        //! @{

        //! Push_back a choice in the list, optionnally give it a callback.
        void add(const std::wstring& text, const Callback callback = nullptr);

        //! Configure previously added choice's text.
        //! The first choice added has ID 0, then ID is increased by one.
        void setChoiceText(uint choice, const std::wstring& text);

        //! Configure previously added choice's callback.
        //! The first choice added has ID 0, then ID is increased by one.
        void setChoiceCallback(uint choice, const Callback callback);

        //! Calls callback from currently selected choice.
        void acceptChoice();

        //! Whether the choice changed since last update.
        inline bool choiceChanged() const { return m_choiceChanged; }

        //! Select a specific choice.
        //! The first choice added has ID 0, then ID is increased by one.
        void selectChoice(uint choice);

        //! Get selected choice ID.
        inline uint selectedChoice() const { return m_selectedChoice; }

        //! Select previous choice in the list.
        void switchChoiceLeft();

        //! Select next choice in the list.
        void switchChoiceRight();

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! Whether the show interactive arrows.
        PARAMGSU(bool, m_showArrows, showArrows, setShowArrows, updateSize)

        //! Whether the show top and bottom lines.
        PARAMGSU(bool, m_showLines, showLines, setShowLines, updateSize)

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void update() override;
        void updateRoutine(const sf::Time& dt) final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        bool handleKeyboardEvent(const sf::Event& event) override;
        void handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        void handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        void handleMouseLeft() final;

        //! @}

        //-----------------//
        //! @name Hovering
        //! @{

        //! Checks if position x is over left arrow.
        bool isLeftArrowSelected(const float& x);

        //! Checks if position x is over right arrow.
        bool isRightArrowSelected(const float& x);

        //! @}

        //-------------------------------//
        //! @name Internal state updates
        //! @{

        //! Update the minimum size needed to fit the bigger choice text and decorum.
        void updateButtonSize();

        //! Update the absolute size.
        //! nui::ImageButton reimplements this to add the image size.
        virtual void updateSize();

        //! @}

        //--------------------------------------//
        //! @name Getters for inherited classes
        //! @{

        //! The currently displayed text.
        sf::Text& text() { return m_text; }

        //! The currently displayed top line (if any).
        sfe::HLine& topLine() { return m_topLine; }

        //! The currently displayed bottom line (if any).
        sfe::HLine& botLine() { return m_botLine; }

        //! The current size allocated to display the text.
        const sf::Vector2f& maxTextSize() const { return m_maxTextSize; }

        //! The current size allocated to display the whole button (text + decorum).
        const sf::Vector2f& buttonSize() const { return m_buttonSize; }

        //! @}

    private:

        //! The choices.
        std::vector<ChoiceInfo> m_choices;

        //! Currently selected choice.
        uint m_selectedChoice = 0u;

        //! Whether the choice has changed since last update.
        bool m_choiceChanged = true;

        sf::Text m_text;        //!< The displayed text.
        sfe::HLine m_topLine;   //!< The displayed top line.
        sfe::HLine m_botLine;   //!< The displayed bottom line.
        sfe::LArrow m_lArrow;   //!< The displayed left arrow.
        sfe::RArrow m_rArrow;   //!< The displayed right arrow.

        //! The max displayed size of all choices.
        sf::Vector2f m_maxTextSize;

        //! The displayed size of this button.
        sf::Vector2f m_buttonSize;

        float m_arrowOffset;    //!< The gap between arrows and text.
        float m_lineOffset;     //!< The gap between lines and text.
        float m_arrowSize;      //!< The width of the arrows.
        float m_lineSize;       //!< The thickness of the lines.
    };
}
