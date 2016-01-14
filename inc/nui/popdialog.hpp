#pragma once

#include "scene/entity.hpp"
#include "nui/frame.hpp"
#include "nui/vstacker.hpp"
#include "nui/hstacker.hpp"
#include "nui/pushbutton.hpp"
#include "scene/wrappers/label.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

#include <memory>

namespace nui
{
    //! A dialog to print a message to the user and force him to answer.
    /*!
     *  Used for important messages such as definitive delete.
     */

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

        std::string _name() const final { return "nui::PopDialog"; }

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
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;

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

        // Content
        nui::Frame m_frame;                                         //!< Frame for the message box.
        scene::Label m_message;                                     //!< The displayed message.
        nui::VStacker m_mainStacker;                                //!< Stacks for message box.
        nui::HStacker m_buttonsStacker;                             //!< Stacks all buttons.
        std::vector<std::unique_ptr<nui::PushButton>> m_buttons;    //!< All buttons, defined by user.

        // Background
        sf::RectangleShape m_background;    //!< Background.
    };
}
