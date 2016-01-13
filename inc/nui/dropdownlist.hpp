#pragma once

#include "nui/dropdownselector.hpp"

namespace nui
{
    //! A selection-button listing choices when clicked.

    class DropDownList final : public nui::Entity
    {
        using baseClass = nui::Entity;

        friend class DropDownSelector;

    public:

        //! Constructor.
        DropDownList();

        //! Default destructor.
        ~DropDownList() = default;

        virtual std::string _name() const override { return "nui::DropDownList"; }

        //--------------------------//
        //! @name Choices management
        //! @{

        //! Push back various choices to the list.
        void add(const std::initializer_list<std::wstring>& texts);

        //! Push back a choice to the list.
        void add(std::wstring string);

        //! Clear all choices, reset the list as default.
        void clear();

        //! Select a specific choice.
        //! The first choice added has ID 0, then ID is increased by one.
        void select(uint choiceID);

        //! Select a choice from its text.
        //! Does nothing if unable to find a matching one.
        void select(const std::wstring& choiceString);

        //! Get the selected choice ID.
        inline uint selected() const { return m_selectedChoiceID; }

        //! Get the selected choice ID.
        inline const std::wstring& selectedText() const { return m_selector.choiceString(m_selectedChoiceID); }

        //! Select the previous choice in the list.
        void selectPrevious();

        //! Select the next choice in the list.
        void selectNext();

        //! @}

        //-----------------//
        //! @name Dropping
        //! @{

        //! Show the list of choices.
        void dropList();

        //! Hide the list of choices.
        void undropList();

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void updateRoutine(const sf::Time& dt) final;
        void onSizeChanges() final;
        void onTransformChanges() final;
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! Update the absolute size.
        void updateSize();

        //! @}

        //---------------//
        //! @name Events
        //! @{

        bool handleKeyboardEvent(const sf::Event& event) final;
        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Refresh text and decorum positions.
        void refresh();

        //! @}

    private:

        // Choices
        uint m_selectedChoiceID = -1u;      //!< The currently selected choice.

        // Selector
        DropDownSelector m_selector;            //!< The choice selector.
        bool m_markForSelectorAttach = false;   //!< Delayer for selector attaching.
        bool m_markForSelectorDetach = false;   //!< Delayer for selector detaching.

        // Text
        sfe::RichText m_text;   //!< The displayed text.

        // Header
        sf::RectangleShape m_headerLeft;    //!< Left part of the header.
        sf::RectangleShape m_headerMiddle;  //!< Middle part of the header.
        sf::RectangleShape m_headerRight;   //!< Right part of the header.
        float m_leftWidth = 0.f;            //!< Width of the left part.
        float m_rightWidth = 0.f;           //!< Width of the right part.

        // Dropped state
        bool m_dropped = false;     //!< Is the list dropped?

        // Decorum
        float m_lineHeight = 0.f;   //!< Fix size of a line height.
        float m_fontSize = 0.f;     //!< Font size as defined in the NUI guides.
        float m_vPadding = 0.f;     //!< Vertical padding as defined in the NUI guides.
        float m_hPadding = 0.f;     //!< Horizontal padding as defined in the NUI guides.
    };
}

