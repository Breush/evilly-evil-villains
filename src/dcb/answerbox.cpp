#include "dcb/answerbox.hpp"

#include "tools/platform-fixes.hpp"
#include "tools/vector.hpp"
#include "tools/debug.hpp"
#include "tools/tools.hpp"

using namespace dcb;

AnswerBox::AnswerBox()
    : m_answerSelectedCallback(nullptr)
{
    // Scroll area
    attachChild(m_scrollArea);
    m_scrollArea.setContent(m_stacker);

    // Background
    addPart(&m_background);
    m_background.setOutlineColor(sf::Color::White);
    m_background.setFillColor({0u, 0u, 0u, 150u});
}

//-------------------//
//----- Routine -----//

void AnswerBox::onSizeChanges()
{
    m_scrollArea.setSize(size());
    refreshTexts();
    refreshParts();
}

void AnswerBox::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_vPadding = cNUI.vPadding;

    refreshParts();
}

//------------------//
//----- Events -----//

bool AnswerBox::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left) false;

    float vOffset = 0.f;

    for (uint i = 0u; i < m_texts.size(); ++i) {
        // Max y for the answer
        vOffset += m_texts[i]->size().y;

        // Feedback on controller if clicked
        if (mousePos.y < vOffset && m_answerSelectedCallback != nullptr) {
            m_answerSelectedCallback(i);
            return true;
        }

        vOffset += m_vPadding;
    }

    return true;
}

//---------------------------//
//----- Answers control -----//

uint AnswerBox::addAnswer(std::vector<std::wstring> choices)
{
    m_answers.emplace_back(std::move(choices));
    return m_answers.size() - 1u;
}

void AnswerBox::showAnswer(uint answerID)
{
    massert(answerID < m_answers.size(), "Answer ID " << answerID << " does not exists.");

    // Remove all previous texts
    m_stacker.unstackAll();
    m_texts.clear();

    // Affects new answer
    for (const auto& answer : m_answers[answerID]) {
        auto text = std::make_unique<scene::WrapLabel<sfe::RichText>>();
        text->setFillColor(sf::Color::White);
        text->setText(answer);
        text->setFont("nui");
        m_texts.emplace_back(std::move(text));
        m_stacker.stackBack(*m_texts.back());
    }

    refreshTexts();
}

void AnswerBox::clearAnswers()
{
    m_answers.clear();
}

//-----------------------------------//
//----- Internal change updates -----//

void AnswerBox::refreshTexts()
{
    for (auto& text : m_texts)
        text->fitWidth(size().x);
}

void AnswerBox::refreshParts()
{
    m_background.setOutlineThickness(m_outlineThickness);
    m_background.setPosition({m_outlineThickness, m_outlineThickness});
    m_background.setSize(size() - 2.f * m_outlineThickness);
}
