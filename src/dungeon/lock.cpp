#include "dungeon/lock.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "tools/vector.hpp"

using namespace dungeon;

Lock::Lock()
{
    // Sprite
    addPart(&m_lock);
    m_lock.setTexture(&Application::context().textures.get(TextureID::DUNGEON_LOCK));

    refreshDisplay();
}

//-------------------//
//----- Routine -----//

void Lock::update()
{
    m_lock.setSize(size());
}

void Lock::refreshDisplay()
{
    config::NUI cNUI;

    sf::Vector2f targetSize{cNUI.hintImageSide, cNUI.hintImageSide};

    setSize(targetSize);
}

//------------------------//
//----- Mouse events -----//

void Lock::handleMouseButtonPressed(const sf::Mouse::Button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
}

void Lock::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    setPartShader(&m_lock, ShaderID::NUI_HOVER);
}

void Lock::handleMouseLeft()
{
    resetPartsShader();
}
