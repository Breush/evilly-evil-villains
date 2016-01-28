#pragma once

namespace nui
{
    template<class Text_t> inline
    ProgressiveText<Text_t>::ProgressiveText()
    {
        addPart(&m_text);
    }

    //-------------------//
    //----- Routine -----//

    template<class Text_t> inline
    void ProgressiveText<Text_t>::updateRoutine(const sf::Time& dt)
    {
        if (!m_running) return;

        m_currentTime += dt;

        // TODO Make this speed configurable
        const sf::Time textSpeed = sf::milliseconds(20);
        while (m_currentTime > textSpeed) {
            m_currentTime -= textSpeed;

            auto currentString = m_text.getString();
            currentString += m_string[currentString.getSize()];
            m_text.setString(currentString);

            if (currentString.getSize() == m_string.size())
                m_running = false;
        }
    }

    //------------------------//
    //----- Text control -----//

    template<class Text_t> inline
    void ProgressiveText<Text_t>::setString(std::wstring string)
    {
        m_string = std::move(string);
        restart();
    }

    template<class Text_t> inline
    void ProgressiveText<Text_t>::restart()
    {
        m_currentTime = sf::Time::Zero;
        m_text.setString(L"");
        m_running = m_string.size() > 0u;
    }
}
