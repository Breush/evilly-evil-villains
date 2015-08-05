#pragma once

#include "tools/int.hpp"
#include "tools/string.hpp"

namespace dcb
{
    // Forward declaration

    class ControlledElement;
    class GaugesManager;
    class Bubble;
    class AnswerBox;

    //! Controls the other DCB elements for the negociation phase.

    class Controller
    {
        friend ControlledElement;

    public:

        //! Constructor.
        Controller(GaugesManager& gaugeManager, Bubble& bubble, AnswerBox& answerBox);

        //! Destructor.
        ~Controller();

        //-------------------------//
        //! @name Sequence control
        //! @{

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

    private:

        GaugesManager& m_gaugesManager; //!< The gauges.
        Bubble& m_bubble;               //!< The place where the question is asked.
        AnswerBox& m_answerBox;         //!< The place where the possible answers are shown.
    };
}
