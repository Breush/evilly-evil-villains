#include "core/application.hpp"

#include "nui/donjon/inter.hpp"
#include "nui/uicore.hpp"

#include "tools/debug.hpp"
#include "tools/event.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

using namespace nui;

DonjonInter::DonjonInter()
{
    // TODO Get from donjoninfo (in game::context)?
    m_grid.setRowsColumns(7, 5);
}

//-------------------//
//----- Routine -----//

void DonjonInter::init()
{
    core()->add(&m_contextMenu);
    m_contextMenu.init();

    update();
}

void DonjonInter::handleMouseEvent(const sf::Event& event, const sf::Vector2f& relPos)
{
    // Mouse events
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == sf::Mouse::Right)
        {
            // Get coords for donjon inter positions
            sf::Vector2f coords = getInverseTransform().transformPoint(relPos);
            sf::Vector2u roomFloor = m_grid.rowColumnFromCoords(coords);

            std::cout << roomFloor.x << ", " << roomFloor.y << std::endl;

            addPart(&m_contextMenu);
        }
    }
}

bool DonjonInter::handleKeyboardEvent(const sf::Event& event)
{
#ifdef DEBUG_MODE
    // Keyboard event
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Add)
            m_grid.setRowsColumns(m_grid.rows() + 1, m_grid.columns());
        else if (event.key.code == sf::Keyboard::Subtract)
            m_grid.setRowsColumns(m_grid.rows() - 1, m_grid.columns());
    }
#endif

    return false;
}

void DonjonInter::update()
{
    clearParts();
    addPart(&m_grid);
}

//-------------------//
//----- Refresh -----//

void DonjonInter::changedSize()
{
    m_grid.setSize(size());
}

