#include "nui/contextmenu.hpp"

#include "core/application.hpp"
#include "tools/tools.hpp"

using namespace nui;

ContextMenu::ContextMenu()
    : baseClass()
{
    setVisible(false);

    // Content
    attachChild(m_frame);
    m_frame.setContent(m_stacker);
    m_stacker.setPadding(0.f);
}

//-------------------//
//----- Routine -----//

void ContextMenu::onChildSizeChanges(scene::Entity& child)
{
    returnif (&m_frame != &child);
    updateSize();
}

void ContextMenu::updateSize()
{
    setSize(m_frame.size());
}

//------------------------//
//----- Mouse events -----//

void ContextMenu::handleGlobalEvent(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed && visible())
        markForVisible(false);
}

bool ContextMenu::handleMouseButtonPressed(const sf::Mouse::Button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    uint selectedChoiceID = choiceIDFromPosition(mousePos);
    returnif (selectedChoiceID >= m_choices.size()) false;

    if (m_choices[selectedChoiceID].callback != nullptr) {
        Application::context().sounds.play("core/nui/accept/accept");
        m_choices[selectedChoiceID].callback();
    }
    else {
        Application::context().sounds.play("core/nui/refuse/refuse");
        markForVisible(true);
    }

    return true;
}

bool ContextMenu::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    uint hoveredChoiceID = choiceIDFromPosition(mousePos);

    if (m_hoveredChoiceID != -1u)
        m_choices[m_hoveredChoiceID].label->setShader("");
    m_hoveredChoiceID = hoveredChoiceID;

    returnif (hoveredChoiceID >= m_choices.size()) false;
    returnif (m_choices[hoveredChoiceID].callback == nullptr) true;

    m_choices[hoveredChoiceID].label->setShader("core/nui/hover/hover");

    return true;
}

void ContextMenu::handleMouseLeft()
{
    m_hoveredChoiceID = -1u;
}

//-------------------//
//----- Choices -----//

void ContextMenu::setTitle(const std::wstring& title)
{
    m_frame.setTitle(title);
}

void ContextMenu::addChoice(const std::wstring& text, Callback callback)
{
    ChoiceInfo choiceInfo;

    choiceInfo.label = std::make_unique<scene::Label>();
    choiceInfo.label->setText(text);
    choiceInfo.label->setPrestyle(scene::Label::Prestyle::NUI);
    choiceInfo.callback = callback;

    m_choices.emplace_back(std::move(choiceInfo));
    m_stacker.stackBack(*m_choices.back().label);
}

void ContextMenu::clearChoices()
{
    m_choices.clear();
}

uint ContextMenu::choiceIDFromPosition(const sf::Vector2f& relPos) const
{
    float yOffset = m_stacker.localPosition().y;
    float yOffsetPrevious = yOffset;

    for (uint choiceID = 0u; choiceID < m_choices.size(); ++choiceID) {
        yOffset += m_choices[choiceID].label->size().y;
        returnif (relPos.y > yOffsetPrevious && relPos.y <= yOffset) choiceID;
        yOffsetPrevious = yOffset;
    }

    return -1u;
}
