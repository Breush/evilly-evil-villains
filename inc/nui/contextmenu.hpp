#pragma once

#include "scene/entity.hpp"

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <sstream>
#include <functional>

namespace nui
{
    //! A simple menu with its own background.
    /*!
     *  Usually called when a right click occured
     *  and the user is given some choices.
     */

    class ContextMenu final : public scene::Entity
    {
        using baseClass = scene::Entity;

    protected:

        //! Type for function callback when a choice is clicked.
        using Callback = std::function<void()>;

        //! A choice is a sf::Text and a function callback.
        struct ChoiceInfo
        {
            sf::Text text;      //!< The text to display when choice is selected.
            Callback callback;  //!< The callback function to call when choice accepted.
        };

    public:

        //! Constructor.
        ContextMenu();

        //! Default destructor.
        ~ContextMenu() = default;

        std::string _name() const final { return "nui::ContextMenu"; }

        //----------------//
        //! @name Choices
        //! @{

        //! The displayed title if string is not empty.
        void setTitle(const std::wstring& title);

        //! Push back a choice in the list, optionnally give it a callback.
        void addChoice(const std::wstring& text, Callback callback = nullptr);

        //! Remove all choices from the list.
        void clearChoices();

        //! The choice hovered at specific coordinates.
        uint choiceFromCoords(const sf::Vector2f& coords) const;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        //! Reacts to a global event, supposed to be directly passed at the state level.
        void handleGlobalEvent(const sf::Event& event);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        //! @}

        //-------------------------------//
        //! @name Internal state updates
        //! @{

        //! Update the absolute size.
        void updateSize();

        //! @}

    private:

        //! The background used, can have a texture.
        sf::RectangleShape m_background;

        //! The title displayed.
        sf::Text m_title;

        //! The list of all choices.
        std::vector<ChoiceInfo> m_choices;

        //! The size of the font.
        float m_fontSize;

        //! The global padding.
        float m_padding;

        //! The height of a choice.
        float m_choiceHeight;
    };
}

