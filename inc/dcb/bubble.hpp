#pragma once

#include "scene/entity.hpp"
#include "sfe/wraptext.hpp"

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

        //! Forces a text to be a certain value and does not register it.
        /*!
         *  This behavior is kind of against the philosophy
         *  of this entity. However this can be useful in some situation.
         */
        void forceText(std::wstring text);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges();
        void refreshDisplay() final;

        //! @}

    private:

        sf::RectangleShape m_background;    //!< The background.
        sfe::WrapText m_wrapText;           //!< The text shown.
        std::vector<std::wstring> m_texts;  //!< The list of texts.
    };
}
