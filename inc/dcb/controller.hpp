#pragma once

#include "dcb/gaugesmanager.hpp"
#include "tools/int.hpp"
#include "tools/string.hpp"

namespace dcb
{
    // Forward declaration

    class Bubble;
    class AnswerBox;

    //! Controls the other DCB elements for the negociation phase.

    class Controller
    {
        using Points = std::array<float, GaugesManager::GaugeID::COUNT>;
        using AnswersPoints = std::vector<Points>;

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

        //! @}

    protected:

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

        //! The points to give to each gauge for each answer of each message.
        std::vector<AnswersPoints> m_points;
    };
}
