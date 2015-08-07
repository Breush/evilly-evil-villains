#include "dcb/controller.hpp"

#include "dcb/debug.hpp"
#include "dcb/bubble.hpp"
#include "dcb/answerbox.hpp"
#include "tools/tools.hpp"

#include <pugixml/pugixml.hpp>

using namespace dcb;

Controller::Controller(GaugesManager& gaugesManager, Bubble& bubble, AnswerBox& answerBox)
    : m_gaugesManager(gaugesManager)
    , m_bubble(bubble)
    , m_answerBox(answerBox)
{
    // Get callback
    m_answerBox.setAnswerSelectedCallback([this](uint selectedAnswer) { onAnswerSelected(selectedAnswer); });
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

void Controller::startSequence()
{
    // Reset and pick new question
    m_questionsSeen.clear();
    continueSequence();
}

void Controller::continueSequence()
{
    // Can't or should not pick anymore
    if (m_questionsSeen.size() == m_questionsLimit || m_questionsSeen.size() == m_points.size()) {
        if (m_onSequenceFinishedCallback != nullptr)
            m_onSequenceFinishedCallback();
        return;
    }

    // Pick
    do { m_selectedQuestion = rand() % m_points.size(); }
    while (m_questionsSeen.find(m_selectedQuestion) != std::end(m_questionsSeen));
    m_questionsSeen.emplace(m_selectedQuestion);

    // Show
    m_bubble.showMessage(m_selectedQuestion);
    m_answerBox.showAnswer(m_selectedQuestion);
    mdebug_dcb_1("Showing question #" << m_selectedQuestion);
}

//-------------------//
//----- Filling -----//

void Controller::load(const std::string& file)
{
    // Clearing all
    m_bubble.clearMessages();
    m_answerBox.clearAnswers();
    m_points.clear();

    // Parsing XML
    pugi::xml_document doc;
    doc.load_file(file.c_str());

    const auto& dcb = doc.child(L"dcb");

    if (!dcb)
        throw std::runtime_error("File " + toString(file) + " is not a valid DCB file.");

    // Get all messages
    for (const auto& message : dcb.children(L"message")) {
        m_bubble.addMessage(message.attribute(L"question").as_string());

        // All answers in the message
        std::vector<std::wstring> answers;
        AnswersPoints answersPoints;
        for (const auto& answer : message.children(L"answer")) {
            answers.emplace_back(answer.child_value());
            addPointsFromType(answersPoints, answer.attribute(L"type").as_string());
        }

        m_answerBox.addAnswer(answers);
        m_points.emplace_back(answersPoints);
    }

    mdebug_dcb_1("Loaded " << m_points.size() << " messages/answers from file " << file);
}

//--------------------//
//----- Feedback -----//

void Controller::onAnswerSelected(uint answerSelected)
{
    mdebug_dcb_1("Selected answer #" << answerSelected);
    m_gaugesManager.addToGauges(m_points[m_selectedQuestion][answerSelected]);
    continueSequence();
}

//-----------------------------//
//----- Points management -----//

void Controller::addPointsFromType(AnswersPoints& answersPoints, const std::wstring& type)
{
    Points points;

    if (type == L"lie") {
        points[GaugesManager::GaugeID::APPRECIATION]    = fmod(rand(), 31.f) - 15.f;
        points[GaugesManager::GaugeID::CONFUSION]       = -fmod(rand(), 6.f);
        points[GaugesManager::GaugeID::TRUST]           = -fmod(rand(), 16.f);
        points[GaugesManager::GaugeID::CONVICTION]      = 15.f + fmod(rand(), 11.f);
    }
    else if (type == L"truth") {
        points[GaugesManager::GaugeID::APPRECIATION]    = 5.f + fmod(rand(), 21.f);
        points[GaugesManager::GaugeID::CONFUSION]       = -fmod(rand(), 6.f);
        points[GaugesManager::GaugeID::TRUST]           = fmod(rand(), 31.f) - 15.f;
        points[GaugesManager::GaugeID::CONVICTION]      = -5.f - fmod(rand(), 16.f);
    }
    else if (type == L"sarcasm") {
        points[GaugesManager::GaugeID::APPRECIATION]    = -fmod(rand(), 16.f);
        points[GaugesManager::GaugeID::CONFUSION]       = fmod(rand(), 5.f) - 15.f;
        points[GaugesManager::GaugeID::TRUST]           = 5.f + fmod(rand(), 21.f);
        points[GaugesManager::GaugeID::CONVICTION]      = -fmod(rand(), 6.f);
    }
    else if (type == L"absurd") {
        points[GaugesManager::GaugeID::APPRECIATION]    = fmod(rand(), 31.f) - 15.f;
        points[GaugesManager::GaugeID::CONFUSION]       = 15.f + fmod(rand(), 21.f);
        points[GaugesManager::GaugeID::TRUST]           = fmod(rand(), 31.f) - 15.f;
        points[GaugesManager::GaugeID::CONVICTION]      = fmod(rand(), 31.f) - 15.f;
    }
    else {
        throw std::runtime_error("Unknown answer type loading DCB file.");
    }

    answersPoints.emplace_back(points);
}
