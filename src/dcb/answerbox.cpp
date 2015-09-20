#include "dcb/answerbox.hpp"

#include "core/application.hpp"
#include "tools/debug.hpp"
#include "tools/vector.hpp"
#include "tools/tools.hpp"

using namespace dcb;

AnswerBox::AnswerBox()
    : m_answerSelectedCallback(nullptr)
{
    // Background
    addPart(&m_background);
    m_background.setOutlineColor(sf::Color::White);
    m_background.setFillColor({0u, 0u, 0u, 150u});
}

//-------------------//
//----- Routine -----//

void AnswerBox::onSizeChanges()
{
    refreshTexts();
    refreshParts();
}

void AnswerBox::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

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
        vOffset += boundsSize(m_texts[i]).y;

        // Feedback on controller if clicked
        if (mousePos.y < vOffset && m_answerSelectedCallback != nullptr) {
            m_answerSelectedCallback(i);
            return true;
        }
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
    for (auto& text : m_texts)
        removePart(&text);

    // Resize
    const auto& answer = m_answers[answerID];
    m_texts.resize(answer.size());

    // Affects new answer
    for (uint i = 0u; i < m_texts.size(); ++i) {
        auto& text = m_texts[i];
        text.setWrapString(answer[i]);
        addPart(&text);
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
    float vOffset = 0.f;

    for (auto& text : m_texts) {
        text.fitWidth(size().x);
        text.setFont(Application::context().fonts.get("nui"));
        text.setColor(sf::Color::White);
        text.setPosition({0.f, vOffset});
        vOffset += boundsSize(text).y;
    }
}

void AnswerBox::refreshParts()
{
    m_background.setOutlineThickness(m_outlineThickness);
    m_background.setPosition({m_outlineThickness, m_outlineThickness});
    m_background.setSize(size() - 2.f * m_outlineThickness);
}
