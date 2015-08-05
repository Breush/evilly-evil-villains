#include "dcb/controller.hpp"

#include "dcb/gaugesmanager.hpp"
#include "dcb/bubble.hpp"
#include "dcb/answerbox.hpp"
#include "tools/tools.hpp"

using namespace dcb;

Controller::Controller(GaugesManager& gaugesManager, Bubble& bubble, AnswerBox& answerBox)
    : m_gaugesManager(gaugesManager)
    , m_bubble(bubble)
    , m_answerBox(answerBox)
{
    m_answerBox.setAnswerSelectedCallback([this](uint selectedAnswer) { onAnswerSelected(selectedAnswer); });

    // Init with all strings...
    /*m_bubble.clearMessages();
    m_answerBox.clearAnswers();
    m_points.clear();*/

    // TODO Get all from a file...
    // If so, how will they be translated?
    m_bubble.addMessage(L"And... how many dungeons have you already managed?");
    // Answers are always added in the same order: Truth/Lie/Sarcasm/Absurd
    // Its the answer that will shuffle the order visually
    // TODO How do we know which one the the truth for this question?
    // -> Maybe this question is forced anyway.
    m_answerBox.addAnswer({L"Well... None... but I feel like I will be the evilest man ever!"
                         , L"I have enough experience in that domain: I managed dozens of dungeons."
                         , L"Hundreds of thousands, for sure! Have you never heard of me?"
                         , L"I was the manager of the city's library last year. I guess it's the same, right?"});
    m_answerBox.showAnswer(0u);
    //m_points.emplace_back(x,y,z,a);
}

Controller::~Controller()
{
    m_answerBox.setAnswerSelectedCallback(nullptr);
}

//----------------------------//
//----- Sequence control -----//

void Controller::randomGaugesFromString(const std::wstring& str)
{
    std::hash<std::wstring> hasher;
    m_gaugesManager.randomGauges(static_cast<float>(hasher(str)), 5.f, 60.f);
}

//--------------------//
//----- Feedback -----//

void Controller::onAnswerSelected(uint answerSelected)
{
    std::wcout << L"[dcb::Controller] Selected answer: " << answerSelected << std::endl;
}
