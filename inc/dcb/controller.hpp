#pragma once

#include "dcb/gaugesmanager.hpp"
#include "tools/int.hpp"
#include "tools/string.hpp"

#include <set>
#include <functional>

namespace dcb
{
    // Forward declaration

    class Bubble;
    class AnswerBox;

    //! Controls the other DCB elements for the negociation phase.

    class Controller
    {
        //! Holding points for each gauge.
        using Points = std::array<float, GaugesManager::GaugeID::COUNT>;

        //! For each answers, points for each gauge.
        using AnswersPoints = std::vector<Points>;

        //! Callback type.
        using Callback = std::function<void()>;

    public:

        //! Constructor.
        Controller(GaugesManager& gaugeManager, Bubble& bubble, AnswerBox& answerBox);

        //! Destructor.
        ~Controller();

        //-------------------------//
        //! @name Sequence control
        //! @{

        //! Loads the content from a file.
        void load(const std::string& file);

        //! Randomize the gaudes from a string.
        void randomGaugesFromString(const std::wstring& str);

        //! Start the sequence of questions.
        //! Requires load() to be called before.
        void startSequence();

        //! Used to warn state when sequence is finished.
        void setOnSequenceFinishedCallback(Callback callback) { m_onSequenceFinishedCallback = callback; }

        //! @}

    protected:

        //-------------------------//
        //! @name Sequence control
        //! @{

        //! Continue the sequence to the next question.
        void continueSequence();

        //! @}

        //-----------------//
        //! @name Feedback
        //! @{

        //! Called by a dcb::ControlledElement when something occured.
        void onAnswerSelected(uint answerSelected);

        //! @}

        //--------------------------//
        //! @name Points management
        //! @{

        void addPointsFromType(AnswersPoints& answersPoints, const std::wstring& type);

        //! @}

    private:

        GaugesManager& m_gaugesManager; //!< The gauges.
        Bubble& m_bubble;               //!< The place where the question is asked.
        AnswerBox& m_answerBox;         //!< The place where the possible answers are shown.

        //! Used to warn state when sequence is finished.
        Callback m_onSequenceFinishedCallback = nullptr;

        //! The points to give to each gauge for each answer of each message.
        std::vector<AnswersPoints> m_points;

        // Questions
        uint m_selectedQuestion = -1u;      //!< Currently selected question.
        const uint m_questionsLimit = 7u;   //!< The number of questions asked if enough.
        std::set<uint> m_questionsSeen;     //!< List of questions already seen.
    };
}
