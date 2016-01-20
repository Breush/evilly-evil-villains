#include "nui/form.hpp"

#include "config/nuiguides.hpp"
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

void Form::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_fontSize = cNUI.fontSize;

    for (auto& label : m_labels)
        label->setCharacterSize(m_fontSize);
}

//-------------------//
//----- Control -----//

void Form::add(std::wstring text, scene::Entity& child)
{
    uint line = m_labels.size();

    m_labels.emplace_back(std::make_unique<nui::Text>());
    m_labels[line]->setString(text);
    m_labels[line]->setFont("core/global/fonts/nui");
    m_labels[line]->setFillColor(sf::Color::White);
    m_labels[line]->setCharacterSize(m_fontSize);

    m_table.setDimensions(m_labels.size(), 2u);
    m_table.setChild(line, 0u, *m_labels[line], Align::OPPOSITE);
    m_table.setChild(line, 1u, child);
}

void Form::add(scene::Entity& child)
{
    add(L"", child);
}

void Form::setText(uint line, std::wstring text)
{
    m_labels[line]->setString(text);
}

void Form::setTooltip(uint line, const std::wstring& tooltip)
{
    m_labels[line]->setTooltip(tooltip);
}
