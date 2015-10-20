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

        //-----------------//
        //! @name Children
        //! @{

        //! Add a new line to the form.
        void add(std::wstring text, scene::Entity& child);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;

        //! @}

    private:

        nui::TableLayout m_table;                           //!< The layout.
        std::vector<std::unique_ptr<scene::Label>> m_labels;  //!< The labels.
    };
}
