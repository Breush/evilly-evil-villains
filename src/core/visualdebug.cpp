#include "core/visualdebug.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"

#include <sstream>

void VisualDebug::init()
{
    refreshDisplay();

    // Getting font from holder
    m_text.setPosition(10.f, 10.f);
    m_text.setCharacterSize(16);
    m_text.setColor(sf::Color::White);
    m_text.setFont(Application::context().fonts.get(FontID::MONO));

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
        str << L"Time factor: " << m_timeFactor;
        m_text.setString(str.str());
        updateBackgroundSize();

        // And reset counters
        m_time -= 1.f;
        m_renderedFrames = 0;
        m_renderedUpdates = 0;
    }
}

void VisualDebug::draw()
{
    returnif (!m_visible);

    // Update internal state
    ++m_renderedFrames;

    // Draw
    auto& window = Application::context().window;
    window.setView(m_view);
    window.draw(m_background);
    window.draw(m_text);
}

void VisualDebug::refreshDisplay()
{
    const auto& resolution = Application::context().resolution;
    const auto& viewport = Application::context().viewport;

    // The view
    m_view.setViewport(viewport);
    m_view.setSize(resolution);
    m_view.setCenter(resolution / 2.f);
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
    }
}
