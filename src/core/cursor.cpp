#include "core/cursor.hpp"

#include "core/application.hpp"
#include "tools/vector.hpp"

void Cursor::init()
{
    // Setting background
    const auto& texture = Application::context().textures.get("nui/cursor/normal");
    m_shape.setSize(sf::v2f(texture.getSize()));
    m_shape.setTexture(&texture);

}

//-------------------//
//----- Routine -----//

void Cursor::update(const sf::Time&)
{
    m_shape.setPosition(sf::v2f(sf::Mouse::getPosition(Application::context().window)));
}

void Cursor::refreshWindow(const config::WindowInfo& cWindow)
{
    m_view.setSize(cWindow.screenSize);
    m_view.setCenter(cWindow.screenSize / 2.f);
}

void Cursor::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Draw
    target.setView(m_view);
    target.draw(m_shape, states);
}
