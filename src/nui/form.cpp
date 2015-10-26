#include "nui/form.hpp"

#include "tools/platform-fixes.hpp" // make_unique

using namespace nui;

Form::Form()
{
    // Table
    attachChild(m_table);
    m_table.setAutoSize(true);
}

//-------------------//
//----- Routine -----//

void Form::onChildSizeChanges(scene::Entity& child)
{
    setSize(m_table.size());
}

//--------------------//
//----- Children -----//

void Form::add(std::wstring text, scene::Entity& child)
{
    uint line = m_labels.size();

    m_labels.emplace_back(std::make_unique<scene::Label>());
    m_labels[line]->setText(text);
    m_labels[line]->setPrestyle(scene::Label::Prestyle::NUI);

    m_table.setDimensions(m_labels.size(), 2u);
    m_table.setChild(line, 0u, *m_labels[line], Align::OPPOSITE);
    m_table.setChild(line, 1u, child);
}
