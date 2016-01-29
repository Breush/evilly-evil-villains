#pragma once

#include "scene/entity.hpp"
#include "sfe/twraptext.hpp"
#include "sfe/richtext.hpp"

#include <functional>

namespace dcb
{
    //! A line to click in the answer box.

    class AnswerLine final : public scene::Entity
    {
        using baseClass = scene::Entity;

        //! Called whenever this answer line is clicked.
        using ClickedCallback = std::function<void()>;

    public:

        //! Constructor.
        AnswerLine();

        //! Default destructor.
        ~AnswerLine() = default;

        std::string _name() const noexcept final { return "dcb::AnswerLine"; }

        //----------------//
        //! @name Control
        //! @{

        //! Set the text of the shown answer.
        void setString(const std::wstring& string);

        //! Set the width of the line.
        void fitWidth(float width);

        //! Called whenever this answer line is clicked.
        inline void setClickedCallback(ClickedCallback clickedCallback) { m_clickedCallback = std::move(clickedCallback); }

        //! @}

    private:

        //----------------//
        //! @name Routine
        //! @{

        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! Update the absolute size.
        void updateSize();

        //! @}

        //---------------//
        //! @name Events
        //! @{

        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        //! @}

    private:

        sfe::WrapText<sfe::RichText> m_text;            //!< The text shown.
        ClickedCallback m_clickedCallback = nullptr;    //!< Called whenever this answer line is clicked.

        // Decorum
        float m_width = 0.f;    //!< The width of the line.
    };
}
