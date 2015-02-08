#include "core/visualdebug.hpp"

#include "core/application.hpp"
#include "tools/tools.hpp"

#include <sstream>

VisualDebug::VisualDebug()
    : m_visible(false)
    , m_renderedFrames(0)
    , m_time(0.f)
{
}

void VisualDebug::init()
{
    // Getting font from holder
    sf::Font& font = Application::context().fonts.get(Fonts::NUI);
    m_text.setPosition(10.f, 10.f);
    m_text.setCharacterSize(16);
    m_text.setFont(font);
}

void VisualDebug::update(const sf::Time& dt)
{
    returnif (!m_visible);

    m_time += dt.asSeconds();

    if (m_time >= 1.f) {
        // Update text each second
        std::wstringstream str;
        str << L"FPS: " << m_renderedFrames << std::endl;
        m_text.setString(str.str());

        // And reset counters
        m_time -= 1.f;
        m_renderedFrames = 0;
    }
}

void VisualDebug::draw()
{
    returnif (!m_visible);

    // Update internal state
    ++m_renderedFrames;

    // Draw
    auto& window = Application::context().window;
    window.draw(m_text);
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
    }
}
