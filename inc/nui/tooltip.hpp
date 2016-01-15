#pragma once

#include "scene/entity.hpp"
#include "scene/wrappers/rectangleshape.hpp"
#include "scene/wrappers/richlabel.hpp"


namespace nui
{
    //! Basically a string in some small box.

    class Tooltip final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        Tooltip();

        //! Destructor.
        ~Tooltip() = default;

        inline std::string _name() const { return "nui::Tooltip"; }

        //----------------//
        //! @name Control
        //! @{

        //! Set the text to be displayed.
        void setText(const std::wstring& text);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Refresh the background to fit the text size.
        void refresh();

        //! @}

    private:

        scene::RichLabel m_text;                //!< The label.
        scene::RectangleShape m_background;     //!< The background.
    };
}
