#include "dcb/answerbox.hpp"

#include "config/nui.hpp"
#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "tools/debug.hpp"
#include "tools/vector.hpp"

using namespace dcb;

AnswerBox::AnswerBox()
{
    setDetectable(false);

    // Background
    addPart(&m_background);
    m_background.setOutlineColor(sf::Color::White);
    m_background.setFillColor({255u, 255u, 255u, 10u});

    refreshDisplay();
}

//-------------------//
//----- Routine -----//

void AnswerBox::onSizeChanges()
{
    refreshTexts();
    refreshParts();
}

void AnswerBox::refreshDisplay()
{
    refreshParts();
    baseClass::refreshDisplay();
}

//---------------------------//
//----- Answers control -----//

uint AnswerBox::addAnswer(std::initializer_list<std::wstring> choices)
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

//-----------------------------------//
//----- Internal change updates -----//

void AnswerBox::refreshTexts()
{
    float vOffset = 0.f;

    for (auto& text : m_texts) {
        text.fitWidth(size().x);
        text.setFont(Application::context().fonts.get(FontID::NUI));
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
