#include "dcb/controller.hpp"

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

    m_answerBox.showAnswer(0u); // FIXME
    std::cerr << "Loaded " << m_points.size() << " answers" << std::endl;
}

//--------------------//
//----- Feedback -----//

void Controller::onAnswerSelected(uint answerSelected)
{
    std::wcout << L"[dcb::Controller] Selected answer: " << answerSelected;

    // TODO Know currently shown answer and replace that zero.
    m_gaugesManager.addToGauges(m_points[0u][answerSelected]);
}

//-----------------------------//
//----- Points management -----//

void Controller::addPointsFromType(AnswersPoints& answersPoints, const std::wstring& type)
{
    Points points;

    if (type == L"lie") {
        points[GaugesManager::GaugeID::APPRECIATION]    = fmod(rand(), 31.f) - 15.f;
        points[GaugesManager::GaugeID::CONFUSION]       = 0.f;
        points[GaugesManager::GaugeID::TRUST]           = -fmod(rand(), 16.f);
        points[GaugesManager::GaugeID::CONVICTION]      = 15.f + fmod(rand(), 11.f);
    }
    else if (type == L"truth") {
        points[GaugesManager::GaugeID::APPRECIATION]    = 5.f + fmod(rand(), 21.f);
        points[GaugesManager::GaugeID::CONFUSION]       = 0.f;
        points[GaugesManager::GaugeID::TRUST]           = fmod(rand(), 31.f) - 15.f;
        points[GaugesManager::GaugeID::CONVICTION]      = -5.f - fmod(rand(), 16.f);
    }
    else if (type == L"sarcasm") {
        points[GaugesManager::GaugeID::APPRECIATION]    = -fmod(rand(), 16.f);
        points[GaugesManager::GaugeID::CONFUSION]       = fmod(rand(), 31.f) - 15.f;
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
