#pragma once

#include "scene/entity.hpp"
#include "nui/tablelayout.hpp"

#include <vector>

// Forward declarations

namespace sfe
{
    class Label;
}

namespace nui
{
    //! A container for label/input pairs.

    class Form final : public scene::Entity
    {
    public:

        //! Constructor.
        Form();

        //! Default destructor.
        ~Form() = default;

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

        void update() final;
        void refreshDisplay() final;

        //! @}

    private:

        nui::TableLayout m_table;                           //!< The layout.
        std::vector<std::unique_ptr<sfe::Label>> m_labels;  //!< The labels.
    };
}
