#pragma once

#include "scene/entity.hpp"
#include "sfe/wraptext.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

#include <functional>
#include <vector>

namespace dcb
{
    // TODO Use a scroll area inside to be sure height is kept.
    //! Shows a set of answers from a predifined list.

    class AnswerBox final : public scene::Entity
    {
        using baseClass = scene::Entity;

        //! Used whenever an answer is selected.
        using AnswerSelectedCallback = std::function<void(uint)>;

    public:

        //! Constructor.
        AnswerBox();

        //! Default destructor.
        ~AnswerBox() = default;

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

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! Used whenever an answer is selected.
        PARAMGS(AnswerSelectedCallback, m_answerSelectedCallback, answerSelectedCallback, setAnswerSelectedCallback)

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges();
        void refreshDisplay() final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        void handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Refresh texts position and size.
        void refreshTexts();

        //! Refresh parts position and size.
        void refreshParts();

        //! @}

    private:

        std::vector<std::vector<std::wstring>> m_answers;   //!< An answers consists in a list of choices.
        std::vector<sfe::WrapText> m_texts;                 //!< All the texts shown.
        // TODO Replace by WrapLabel so that this can be added to a stacker

        // Decorum
        sf::RectangleShape m_background;    //!< The background.
        float m_outlineThickness = 1.f; //!< Size of outline.
    };
}
