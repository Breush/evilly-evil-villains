#pragma once

#include "scene/entity.hpp"
#include "sfe/twraptext.hpp"
#include "sfe/richtext.hpp"
#include "nui/progressivetext.hpp"
#include "nui/frame.hpp"

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

        std::string _name() const final { return "dcb::Bubble"; }

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

        //----------------//
        //! @name Display
        //! @{

        //! Set the visible width of the entity (except the frame padding).
        void fitWidth(float width);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onChildSizeChanges(scene::Entity& child) final;
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! @}

    private:

        std::vector<std::wstring> m_messages;   //!< The list of texts.

        // Content
        using Text = sfe::WrapText<sfe::RichText>;
        nui::Frame m_frame;                     //!< Box around the text.
        nui::ProgressiveText<Text> m_text;      //!< The text shown.
    };
}
