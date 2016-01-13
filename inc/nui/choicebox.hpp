#pragma once

#include "nui/entity.hpp"
#include "tools/int.hpp"
#include "sfe/line.hpp"
#include "sfe/arrow.hpp"
#include "sfe/richtext.hpp"

#include <vector>
#include <string>
#include <functional>

namespace nui
{
    //! @deprecated A multi-choices interactive box.
    /*!
     *  This multi-choices button makes a excellent
     *  UI object to let the user choose between
     *  a few options.
     */

    class ChoiceBox : public Entity
    {
        using baseClass = Entity;

    protected:

        //! Type for function callback when button is clicked.
        using Callback = std::function<void()>;

    public:

        //! Constructor.
        ChoiceBox();

        //! Default destructor.
        virtual ~ChoiceBox() = default;

        virtual std::string _name() const override { return "nui::ChoiceBox"; }

        //----------------//
        //! @name Choices
        //! @{

        //! Push back various choices.
        void add(const std::initializer_list<std::wstring>& texts);

        //! Push back a choice in the list, optionnally give it a callback.
        void add(const std::wstring& text, const Callback callback = nullptr);

        //! Configure previously added choice's text.
        //! The first choice added has ID 0, then ID is increased by one.
        void setChoiceText(uint choice, const std::wstring& text);

        //! Configure previously added choice's callback.
        //! The first choice added has ID 0, then ID is increased by one.
        void setChoiceCallback(uint choice, const Callback callback);

        //! Clear all choices, resets the box as default.
        void clearChoices();

        //! Calls callback from currently selected choice.
        void acceptChoice();

        //! Whether the choice changed since last update.
        inline bool choiceChanged() const { return m_choiceChanged; }

        //! Select a specific choice.
        //! The first choice added has ID 0, then ID is increased by one.
        void selectChoice(uint choice);

        //! Select a choice from its text.
        //! Does nothing if unable to find a matching one.
        void selectChoice(const std::wstring& choiceText);

        //! Get the selected choice ID.
        inline uint selectedChoice() const { return m_selectedChoice; }

        //! Get the selected choice string.
        inline std::wstring selectedChoiceText() const { return m_text.getString().toWideString(); }

        //! Select previous choice in the list.
        void switchChoiceLeft();

        //! Select next choice in the list.
        void switchChoiceRight();

        //! @}

        //----------------//
        //! @name Setters
        //! @{

        //! Whether the show arrows.
        void showArrows(bool enabled);

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! Whether the show interactive arrows.
        PARAMG(bool, m_arrowsShowed, arrowsShowed)

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() override;
        void updateRoutine(const sf::Time& dt) final;
        void refreshNUI(const config::NUIGuides& cNUI) override;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        inline bool handleGlobalEvents() const final { return true; }
        void handleGlobalMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f& nuiPos) override;
        bool handleKeyboardEvent(const sf::Event& event) override;
        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        bool handleMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        bool handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
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

        //! Refresh all text and arrows positions.
        void refresh();

        //! Refresh text position.
        void refreshText();

        //! Refresh arrows position.
        void refreshArrows();

        //! Update the minimum size needed to fit the bigger choice text and decorum.
        void updateButtonSize();

        //! Update the absolute size.
        //! nui::ImageButton reimplements this to add the image size.
        virtual void updateSize();

        //! @}

        //--------------------------------------//
        //! @name Getters for inherited classes
        //! @{

        //!< Whether the button is being pressed.
        bool& pressed() { return m_pressed; }

        //! The currently displayed text.
        sfe::RichText& text() { return m_text; }

        //! The displayed button background.
        sf::Sprite& buttonSprite() { return m_buttonSprite; }

        //! The current size allocated to display the text.
        const sf::Vector2f& maxTextSize() const { return m_maxTextSize; }

        //! The current size allocated to display the whole button (text + decorum).
        const sf::Vector2f& buttonSize() const { return m_buttonSize; }

        //! @}

        //! A choice is a wide string and a function callback.
        struct ChoiceInfo
        {
            std::wstring text;  //!< The text to display when choice is selected.
            Callback callback;  //!< The callback function to call when choice accepted.
        };

    private:

        //! The choices.
        std::vector<ChoiceInfo> m_choices;

        //! Currently selected choice.
        uint m_selectedChoice = -1u;

        //! Whether the choice has changed since last update.
        bool m_choiceChanged = true;

        bool m_pressed = false; //!< Whether the button is being pressed.

        sf::Sprite m_buttonSprite;  //!< The background.
        sfe::RichText m_text;       //!< The displayed text.
        sfe::LArrow m_lArrow;       //!< The displayed left arrow.
        sfe::RArrow m_rArrow;       //!< The displayed right arrow.

        //! The max displayed size of all choices.
        sf::Vector2f m_maxTextSize;

        //! The displayed size of this button.
        sf::Vector2f m_buttonSize;

        float m_arrowOffset = 0.f;  //!< The gap between arrows and text.
        float m_arrowSize = 0.f;    //!< The width of the arrows.
        float m_fontSize = 0.f;     //!< The font size used.
    };
}
