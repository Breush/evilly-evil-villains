#pragma once

#include "nui/entity.hpp"

namespace nui
{
    //! A way to show an text letter by letter.

    template<class Text_t>
    class ProgressiveText final : public nui::Entity
    {
        using baseClass = nui::Entity;

    public:

        //! Constructor.
        ProgressiveText();

        //! Default destructor.
        ~ProgressiveText() = default;

        std::string _name() const noexcept final { return "nui::ProgressiveText"; }

        //---------------------//
        //! @name Text control
        //! @{

        //! Set the final string.
        void setString(std::wstring string);

        //! Reset the text, that will be reshown.
        void restart();

        //! Access the text.
        Text_t& text() { return m_text; }

        //! Access the text (const).
        const Text_t& text() const { return m_text; }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void updateRoutine(const sf::Time& dt) final;

        //! @}

    private:

        std::wstring m_string;  //!< The original string.
        Text_t m_text;          //!< The text drawable.

        sf::Time m_currentTime = sf::Time::Zero;    //!< Animation time.
        bool m_running = false;                     //!< Are we running, updated the visible text?
    };
}

#include "nui/progressivetext.inl"
