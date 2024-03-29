#include "core/visualdebug.hpp"

#include "context/context.hpp"
#include "tools/tools.hpp"

#include <sstream>

void VisualDebug::init()
{
    // Getting font from holder
    m_text.setPosition(10.f, 10.f);
    m_text.setFillColor(sf::Color::White);
    m_text.setFont(context::context.fonts.get("core/global/fonts/mono"));

    // Setting background
    m_background.setPosition(0.f, 0.f);
    m_background.setFillColor({0, 0, 0, 220});
}

//-------------------//
//----- Routine -----//

void VisualDebug::update(const sf::Time& dt)
{
    returnif (!m_visible);

    ++m_renderedUpdates;
    m_time += dt.asSeconds();

    if (m_time >= 1.f) {
        // Update text each second
        std::wstringstream str;
        str << L"FPS: " << m_renderedFrames << L" [" << m_renderedUpdates << L"]" << std::endl;
        str << L"Time factor: " << m_timeFactor << std::endl;
        if (m_renderedUpdates != 0u)
            str << L"Average LTT (µs): " << m_logicTickTimeSum / m_renderedUpdates << std::endl;
        if (m_renderedFrames != 0u)
            str << L"Average RTT (µs): " << m_renderTickTimeSum / m_renderedFrames;
        m_text.setString(str.str());
        updateBackgroundSize();

        // And reset counters
        m_time -= 1.f;
        m_renderedFrames = 0;
        m_renderedUpdates = 0;
        m_logicTickTimeSum = 0;
        m_renderTickTimeSum = 0;
    }
}

void VisualDebug::setLogicTickTime(const sf::Time& t)
{
    returnif (!m_visible);

    m_logicTickTimeSum += t.asMicroseconds();
}

void VisualDebug::setRenderTickTime(const sf::Time& t)
{
    returnif (!m_visible);

    m_renderTickTimeSum += t.asMicroseconds();
}

void VisualDebug::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    returnif (!m_visible);

    // Update internal state
    ++const_cast<uint&>(m_renderedFrames);

    // Draw
    target.setView(m_view);
    target.draw(m_background, states);
    target.draw(m_text, states);
}

void VisualDebug::refreshWindow(const config::WindowInfo& cWindow)
{
    m_view = cWindow.referenceView;
}

void VisualDebug::refreshNUI(const config::NUIGuides& cNUI)
{
    m_text.setCharacterSize(cNUI.fontSize);
}

//-----------------------------------//
//----- Internal change updates -----//

void VisualDebug::updateBackgroundSize()
{
    // Making background to text size
    const auto& bounds = m_text.getLocalBounds();
    m_background.setSize({20.f + bounds.left + bounds.width, 20.f + bounds.top + bounds.height});
}

//----------------------//
//----- Visibility -----//

void VisualDebug::switchVisible()
{
    m_visible = !m_visible;

    if (m_visible) {
        m_text.setString(L"FPS: ...");
        updateBackgroundSize();

        m_time = 0.f;
        m_renderedFrames = 0;
        m_renderedUpdates = 0;
        m_logicTickTimeSum = 0;
        m_renderTickTimeSum = 0;
    }
}
