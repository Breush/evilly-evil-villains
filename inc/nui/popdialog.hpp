#pragma once

#include "scene/entity.hpp"
#include "nui/vstacker.hpp"
#include "nui/hstacker.hpp"
#include "nui/button.hpp"
#include "sfe/label.hpp"
#include "sfe/rectangleshape.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <memory> // std::unique_ptr

namespace nui
{
    //! A dialog to print a message to the user and force him to answer.

    class PopDialog final : public scene::Entity
    {
        using baseClass = scene::Entity;

    protected:

        //! Type for function callback.
        using Callback = std::function<void()>;

    public:

        //! Constructor.
        PopDialog();

        //! Destructor.
        ~PopDialog();

        //----------------//
        //! @name Control
        //! @{

        //! Pop up the dialog, will control the whole screen.
        void open();

        //! Whether the dialog is visible or not.
        bool opened() { return visible(); }

        //! Close the the dialog.
        void close();

        //! Reset the diaog as if it was new.
        void reset();

        //! Set the title of dialog.
        void setTitle(const std::wstring& title);

        //! Set the message of dialog.
        void setMessage(const std::wstring& message);

        //! Adds a button to the dialog.
        void addButton(std::wstring text, Callback callback);

        //! What happens when the user clicks outside the message-box or press Escape.
        void setExitCallback(Callback callback);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void onChildSizeChanges() final;
        void refreshDisplay() final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        void handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Refresh message box background size and position.
        void refreshBoxBackground();

        //! @}

    private:

        // State
        Callback m_exitCallback = nullptr;  //!< The function called on outside click or when escape is pressed.

        // Decorum
        sf::RectangleShape m_background;                        //!< Background.
        sfe::RectangleShape m_boxBackground;                    //!< Background for message box.
        sfe::Label m_title;                                     //!< The title of the message box.
        sfe::Label m_message;                                   //!< The displayed message.
        nui::VStacker m_mainStacker;                            //!< Stacks for message box.
        nui::HStacker m_buttonsStacker;                         //!< Stacks all buttons.
        std::vector<std::unique_ptr<nui::Button>> m_buttons;    //!< All buttons, defined by user.
    };
}