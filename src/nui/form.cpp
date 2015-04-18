#include "nui/form.hpp"

#include "config/nui.hpp"
#include "sfe/label.hpp"
#include "tools/platform-fixes.hpp" // make_unique

using namespace nui;

Form::Form()
{
    refreshDisplay();
    attachChild(m_table);
}

//-------------------//
//----- Routine -----//

void Form::update()
{
    m_table.setSize(size());
}

void Form::refreshDisplay()
{
    config::NUI cNUI;

    m_table.setDimensions(0u, 2u, 2.2f * cNUI.fontVSpace);

    baseClass::refreshDisplay();
}

//--------------------//
//----- Children -----//

void Form::add(std::wstring text, scene::Entity& child)
{
    uint line = m_labels.size();

    m_labels.emplace_back(std::make_unique<sfe::Label>());
    m_labels[line]->setText(text);
    m_labels[line]->setPrestyle(sfe::Label::Prestyle::NUI);

    m_table.setChild(line, 0u, *m_labels[line], Align::OPPOSITE);
    m_table.setChild(line, 1u, child);
}

#include "nui/form.hpp"

using namespace nui;

