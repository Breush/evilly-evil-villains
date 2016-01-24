#pragma once

#include "scene/entity.hpp"
#include "scene/wrappers/label.hpp"
#include "nui/frame.hpp"
#include "nui/vstacker.hpp"

#include <SFML/Window/Event.hpp>

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

        //! The choice hovered at specific position.
        uint choiceIDFromPosition(const sf::Vector2f& relPos) const;

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

        void onChildSizeChanges(scene::Entity& child);

        //! Update the absolute size.
        void updateSize();

        //! @}

        //---------------//
        //! @name Events
        //! @{

        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        //! @}

        //! A choice is a sf::Text and a function callback.
        struct ChoiceInfo
        {
            std::unique_ptr<scene::Label> label;    //!< The text to display when choice is selected.
            Callback callback;                      //!< The callback function to call when choice accepted.
        };

    private:

        std::vector<ChoiceInfo> m_choices;  //!< The list of all choices.

        // Content
        nui::Frame m_frame;         //!< Context menu frame.
        nui::VStacker m_stacker;    //!< Stack all the choices.

        // Hovering
        uint m_hoveredChoiceID = -1u;   //!< The hovered choice.
    };
}

