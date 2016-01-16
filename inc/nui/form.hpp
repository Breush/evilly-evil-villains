#pragma once

#include "scene/entity.hpp"
#include "nui/tablelayout.hpp"
#include "nui/text.hpp"

#include <vector>

namespace nui
{
    //! A container for label/input pairs.

    class Form final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        Form();

        //! Default destructor.
        ~Form() = default;

        std::string _name() const final { return "nui::Form"; }

        //----------------//
        //! @name Control
        //! @{

        //! Add a new line to the form.
        void add(std::wstring text, scene::Entity& child);

        //! Add a new line to the form, with empty label.
        void add(scene::Entity& child);

        //! Set the label of a line.
        void setText(uint line, std::wstring text);

        //! Set the tooltip associated to the text.
        void setTooltip(uint line, const std::wstring& tooltip);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onChildSizeChanges(scene::Entity& child) final;
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! @}

    private:

        nui::TableLayout m_table;                           //!< The layout.
        std::vector<std::unique_ptr<nui::Text>> m_labels;   //!< The labels.

        // Decorum
        float m_fontSize = 0.f; //!< Font size as defined in the NUI guides.
    };
}
