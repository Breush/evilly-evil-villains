#include "dcb/answerbox.hpp"

#include "tools/platform-fixes.hpp"
#include "tools/vector.hpp"
#include "tools/debug.hpp"
#include "tools/tools.hpp"

using namespace dcb;

AnswerBox::AnswerBox()
    : m_answerSelectedCallback(nullptr)
{
    // Content
    attachChild(m_frame);
    m_frame.setContent(m_scrollArea);
    m_scrollArea.setContent(m_stacker);
}

//-------------------//
//----- Routine -----//

void AnswerBox::onSizeChanges()
{
    refreshContainers();
    refreshTexts();
}

void AnswerBox::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);
    refreshContainers();
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
    m_answerLines.clear();

    // Affects new answer
    const auto& answers = m_answers[answerID];
    for (uint i = 0u; i < answers.size(); ++i) {
        const auto& answer = answers[i];

        auto pAnswerLine = std::make_unique<AnswerLine>();
        pAnswerLine->setString(answer);
        pAnswerLine->setClickedCallback([this, i] () {
            returnif (m_answerSelectedCallback == nullptr);
            m_answerSelectedCallback(i);
        });

        m_answerLines.emplace_back(std::move(pAnswerLine));
        m_stacker.stackBack(*m_answerLines.back());
    }

    refreshTexts();
}

void AnswerBox::clearAnswers()
{
    m_answers.clear();
}

//---------------//
//----- ICU -----//

void AnswerBox::refreshContainers()
{
    m_scrollArea.setSize(size() - 2.f * m_frame.paddings());
}

void AnswerBox::refreshTexts()
{
    for (auto& pAnswerLine : m_answerLines)
        pAnswerLine->fitWidth(m_scrollArea.size().x);
}
