#pragma once

#include "nui/entity.hpp"
#include "nui/scrollarea.hpp"
#include "nui/tablelayout.hpp"

namespace nui
{
    // Forward declarations

    class DropDownList;

    //! [Internal] The displayed list of choices.
    //! Supposed to be the child of a DropDownList.

    class DropDownSelector final : public nui::Entity
    {
        using baseClass = nui::Entity;

        friend class DropDownList;

    public:

        //! Constructor.
        DropDownSelector(DropDownList& dropDownList);

        //! Default destructor.
        ~DropDownSelector() = default;

        virtual std::string _name() const override { return "nui::DropDownSelector"; }

        //--------------------------//
        //! @name Choices management
        //! @{

        //! Push back a choice to the list.
        void add(std::wstring string);

        //! Clear all choices.
        void clear();

        //! Get a choice string from its ID.
        inline const std::wstring& choiceString(uint choiceID) const { return m_choices[choiceID].string; }

        //! Get the number of choices.
        inline uint choicesCount() const { return m_choices.size(); }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! Update the absolute size.
        void updateSize();

        //! @}

        //---------------//
        //! @name Events
        //! @{

        inline bool handleGlobalEvents() const final { return true; }
        void handleGlobalMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& nuiPos) final;
        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        //! @}

        //-----------------//
        //! @name Hovering
        //! @{

        //! Set the choice being hovered.
        void hoverChoice(uint choice);

        //! Remove the hover highlight.
        void clearHoveredChoice();

        //! Set the rect to highlight the hover.
        void setHoverRect(const sf::FloatRect& rect);

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Refresh texts and decorum positions.
        void refresh();

        //! @}

        //! A choice.
        struct ChoiceInfo
        {
            std::wstring string;                        //!< The choice's string.
            std::unique_ptr<scene::RichLabel> label;    //!< The displayed choice text.
        };

    private:

        DropDownList& m_dropDownList;   //!< The binded DropDownList entity.

        // Choices
        std::vector<ChoiceInfo> m_choices;  //!< All the choices.

        // Content
        nui::ScrollArea m_scrollArea;       //!< Scroll area.
        nui::TableLayout m_tableLayout;     //!< Table layout to position the labels.

        // Fill
        sf::RectangleShape m_fillLeft;      //!< Fill left part.
        sf::RectangleShape m_fillMiddle;    //!< Fill middle part.
        sf::RectangleShape m_fillRight;     //!< Fill right part.
        float m_leftWidth = 0.f;            //!< Width of the left part.
        float m_rightWidth = 0.f;           //!< Width of the right part.

        // Bottom
        sf::RectangleShape m_bottomLeft;    //!< Bottom left part.
        sf::RectangleShape m_bottomMiddle;  //!< Bottom middle part.
        sf::RectangleShape m_bottomRight;   //!< Bottom right part.
        float m_bottomHeight;               //!< Height of the bottom part of the list.

        // Highlights
        sf::RectangleShape m_hoverHighlight;        //!< When the mouse is over.
        uint m_hoveredChoiceID = -1u;               //!< The hovered line.

        // Decorum
        float m_lineHeight = 0.f;   //!< Fix size of a line height.
        float m_hPadding = 0.f;     //!< Horizontal padding as defined in the NUI guides.
        float m_vPadding = 0.f;     //!< Vertical padding as defined in the NUI guides.
    };
}

