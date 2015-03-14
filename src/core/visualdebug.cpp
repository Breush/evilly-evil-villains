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
    m_text.setFont(Application::context().fonts.get(FontID::MONO));
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
        str << L"FPS: " << m_renderedFrames
            << L" [" << m_renderedUpdates << L"]" << std::endl;
        m_text.setString(str.str());

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

//----------------------//
//----- Visibility -----//

void VisualDebug::switchVisible()
{
    m_visible = !m_visible;

    if (m_visible) {
        m_text.setString(L"FPS: ...");
        m_time = 0.f;
        m_renderedFrames = 0;
        m_renderedUpdates = 0;
    }
}
