#include "core/cursor.hpp"

#include "core/application.hpp"
#include "tools/vector.hpp"

void Cursor::init()
{
    setMode("normal");
}

//-------------------//
//----- Routine -----//

void Cursor::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Draw
    target.setView(m_view);
    target.draw(m_shape, states);
}

void Cursor::update(const sf::Time&)
{
    m_shape.setPosition(sf::v2f(sf::Mouse::getPosition(Application::context().window)));
}

void Cursor::refreshWindow(const config::WindowInfo& cWindow)
{
    m_view.setSize(cWindow.screenSize);
    m_view.setCenter(cWindow.screenSize / 2.f);
}

void Cursor::handleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left)
            setMode("pressed");
    }

    else if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left)
            setMode("normal");
    }
}

//-------------------//
//----- Control -----//

void Cursor::setMode(const std::string& mode)
{
    const auto& texture = Application::context().textures.get("nui/cursor/" + mode);
    m_shape.setSize(sf::v2f(texture.getSize()));
    m_shape.setTexture(&texture);
}