#pragma once

#include "dcb/answerline.hpp"
#include "nui/scrollarea.hpp"
#include "nui/vstacker.hpp"
#include "nui/frame.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

#include <memory>
#include <vector>

namespace dcb
{
    //! Shows a set of answers from a predifined list.

    class AnswerBox final : public scene::Entity
    {
        using baseClass = scene::Entity;

        //! Called whenever an answer is selected.
        using AnswerSelectedCallback = std::function<void(uint)>;

    public:

        //! Constructor.
        AnswerBox();

        //! Default destructor.
        ~AnswerBox() = default;

        std::string _name() const final { return "dcb::AnswerBox"; }

        //------------------------//
        //! @name Answers control
        //! @{

        //! Add an answer with multiple choices.
        //! @return The answer ID.
        uint addAnswer(std::vector<std::wstring> choices);

        //! Show a specified answer from its ID.
        void showAnswer(uint answerID);

        //! Remove all answers.
        void clearAnswers();

        //! Called whenever an answer is selected.
        inline void setAnswerSelectedCallback(AnswerSelectedCallback answerSelectedCallback) { m_answerSelectedCallback = answerSelectedCallback; }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges();
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Refresh containers position and size.
        void refreshContainers();

        //! Refresh texts size.
        void refreshTexts();

        //! @}

    private:

        std::vector<std::vector<std::wstring>> m_answers;           //!< An answers consists in a list of choices.
        AnswerSelectedCallback m_answerSelectedCallback = nullptr;  //!< Called whenver an answer is selected.

        // Content
        nui::Frame m_frame;                                     //!< The frame around.
        nui::ScrollArea m_scrollArea;                           //!< The scroll area.
        nui::VStacker m_stacker;                                //!< Stacker inside the scroll area.
        std::vector<std::unique_ptr<AnswerLine>> m_answerLines; //!< All the texts shown.
    };
}
