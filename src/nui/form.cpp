#include "nui/form.hpp"

#include "scene/wrappers/label.hpp"
#include "tools/platform-fixes.hpp" // make_unique

using namespace nui;

Form::Form()
{
    // Table
    attachChild(m_table);
}

//-------------------//
//----- Routine -----//

void Form::onSizeChanges()
{
    m_table.setSize(size());
}

//--------------------//
//----- Children -----//

void Form::add(std::wstring text, scene::Entity& child)
{
    uint line = m_labels.size();

    m_labels.emplace_back(std::make_unique<sfe::Label>());
    m_labels[line]->setText(text);
    m_labels[line]->setPrestyle(sfe::Label::Prestyle::NUI);

    m_table.setDimensions(m_labels.size(), 2u);
    m_table.setRowAdapt(line, Adapt::FIT);
    m_table.setChild(line, 0u, *m_labels[line], Align::OPPOSITE);
    m_table.setChild(line, 1u, child);
}
