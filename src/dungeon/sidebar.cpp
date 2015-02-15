#include "dungeon/sidebar.hpp"

#include "core/application.hpp"

using namespace dungeon;

Sidebar::Sidebar()
    : baseClass(true)
{
    // Background
    // TODO Big cheating here on rotation, better have a vertical texture too
    m_background.setTexture(&Application::context().textures.get(Textures::DUNGEON_PANEL_BACKGROUND));
    m_background.setRotation(90);

    // Reduced button
    m_switchReducedButton.setTexture(&Application::context().textures.get(Textures::DUNGEON_PANEL_SWITCH));
    m_switchReducedButton.setPosition({45.f, 25.f});
    m_switchReducedButton.setSize({20.f, 20.f});
    m_switchReducedButton.setRotation(90);

    update();
}

//-------------------//
//----- Routine -----//

void Sidebar::update()
{
    clearParts();

    // Background
    m_background.setPosition({size().x, 0.f});
    m_background.setSize({size().y, size().x});
    addPart(&m_background);

    // Reduced button
    addPart(&m_switchReducedButton);
}

//------------------------//
//----- Mouse events -----//

void Sidebar::handleMouseButtonPressed(const sf::Mouse::Button&, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    if (m_switchReducedButton.getGlobalBounds().contains(mousePos))
        switchReduced();
}

void Sidebar::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    resetPartsShader();

    if (m_switchReducedButton.getGlobalBounds().contains(mousePos))
        setPartShader(&m_switchReducedButton, Shaders::NUI_HOVER);
}

void Sidebar::handleMouseLeft()
{
    resetPartsShader();
}

//------------------------//
//----- Reduced mode -----//

void Sidebar::setReduced(bool reduced)
{
    m_reduced = reduced;

    if (m_reduced) lerpable()->setTargetPositionOffset({size().x - 45.f,0.f});
    else lerpable()->setTargetPositionOffset({0.f, 0.f});
}

void Sidebar::switchReduced()
{
    setReduced(!m_reduced);
}

void Sidebar::immediateReduce()
{
    setReduced(true);
    setLocalPosition(lerpable()->targetPosition());
}
