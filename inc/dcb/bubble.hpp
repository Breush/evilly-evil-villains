#pragma once

#include "scene/entity.hpp"
#include "sfe/twraptext.hpp"
#include "sfe/richtext.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

#include <vector>
#include <string>

namespace dcb
{
    //! Shows a text from a predifined list.

    class Bubble final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        Bubble();

        //! Default destructor.
        ~Bubble() = default;

        //--------------------------//
        //! @name Text manipulation
        //! @{

        //! Forces the message to be a certain value and does not register it.
        /*!
         *  This behavior is kind of against the philosophy
         *  of this entity. However this can be useful in some situation.
         */
        void forceMessage(const std::wstring& message);

        //! Add a message.
        //! @return The message ID.
        uint addMessage(std::wstring message);

        //! Show a specified message from its ID.
        void showMessage(uint messageID);

        //! Remove all messages.
        void clearMessages();

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges();
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Refresh parts position and size.
        void refreshParts();

        //! @}

    private:

        sfe::WrapText<sfe::RichText> m_wrapText;    //!< The text shown.
        std::vector<std::wstring> m_messages;       //!< The list of texts.

        // Decorum
        sf::RectangleShape m_background;    //!< The background.
        float m_hPadding;   //!< Horizontal padding value got from config.
        float m_vPadding;   //!< Vertical padding value got from config.
        float m_outlineThickness = 1.f; //!< Size of outline.
    };
}
