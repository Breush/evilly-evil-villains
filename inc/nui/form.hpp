#pragma once

#include "scene/entity.hpp"
#include "scene/wrappers/label.hpp"
#include "nui/tablelayout.hpp"

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

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onChildSizeChanges(scene::Entity& child) final;

        //! @}

    private:

        nui::TableLayout m_table;                             //!< The layout.
        std::vector<std::unique_ptr<scene::Label>> m_labels;  //!< The labels.
    };
}
