#include "core/application.hpp"

#include "nui/donjon/inter.hpp"
#include "nui/uicore.hpp"

#include "tools/debug.hpp"
#include "tools/event.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <sstream>

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
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Right) {
            // Get coords for donjon inter positions
            sf::Vector2f coords = getInverseTransform().transformPoint(relPos);
            m_selectedRoom = m_grid.rowColumnFromCoords(coords);

            std::stringstream str;
            str << "Example choice" << std::endl;
            str << "Destroy room " << m_selectedRoom.x << ", " << m_selectedRoom.y << std::endl;
            str << "Create ladder here";

            m_contextMenu.setPosition(coords);
            m_contextMenu.setMessage(str.str());

            addPart(&m_contextMenu);
        }
        else {
            removePart(&m_contextMenu);
        }
    }
}

bool DonjonInter::handleKeyboardEvent(const sf::Event& event)
{
#if defined(DEBUG_MODE)
    // Keyboard event
    if (event.type == sf::Event::KeyPressed) {
        // Add rows
        if (event.key.code == sf::Keyboard::Add)
            m_grid.setRowsColumns(m_grid.rows() + 1, m_grid.columns());
        else if (event.key.code == sf::Keyboard::Subtract)
            m_grid.setRowsColumns(m_grid.rows() - 1, m_grid.columns());

        // Add columns
        else if (event.key.code == sf::Keyboard::Multiply)
            m_grid.setRowsColumns(m_grid.rows(), m_grid.columns() + 1);
        else if (event.key.code == sf::Keyboard::Divide)
            m_grid.setRowsColumns(m_grid.rows(), m_grid.columns() - 1);
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

