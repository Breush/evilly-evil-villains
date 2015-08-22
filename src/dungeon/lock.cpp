#include "dungeon/lock.hpp"

#include "core/application.hpp"
#include "tools/vector.hpp"
#include "dungeon/data.hpp"

using namespace dungeon;

Lock::Lock()
{
    // Sprite
    addPart(&m_lock);
    m_lock.setTexture(Application::context().textures.get("dungeon/panel/lock"));
}

//-------------------//
//----- Routine -----//

void Lock::onSizeChanges()
{
    const auto& textureSize = m_lock.getTexture()->getSize();
    m_lock.setScale(size() / sf::v2f(textureSize));
}

void Lock::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    sf::Vector2f targetSize{cNUI.hintImageSide, cNUI.hintImageSide};

    setSize(targetSize);
}

//------------------------//
//----- Mouse events -----//

void Lock::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f&, const sf::Vector2f&)
{
    if (button == sf::Mouse::Left)
        switchMode();
}

void Lock::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    setPartShader(&m_lock, "nui/hover");
}

void Lock::handleMouseLeft()
{
    resetPartsShader();
}

//-----------------------//
//----- Data events -----//

void Lock::receive(const Event& event)
{
    if (event.type == EventType::MODE_CHANGED)
        updateMode();
}

void Lock::useData(Data& data)
{
    m_data = &data;
    setEmitter(&data);
    updateMode();
}

//---------------------------//
//----- Mode management -----//

void Lock::updateMode()
{
    switch (m_data->mode()) {
    case Mode::DESIGN:
        m_lock.setColor(sf::Color::White);
        break;

    case Mode::INVASION:
        m_lock.setColor(sf::Color::Red);
        break;
    }
}

void Lock::switchMode()
{
    switch (m_data->mode()) {
    case Mode::DESIGN:
        m_data->setMode(Mode::INVASION);
        break;

    case Mode::INVASION:
        m_data->setMode(Mode::DESIGN);
        break;
    }
}
