#include "dungeon/inter.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "nui/uicore.hpp"
#include "tools/debug.hpp"
#include "tools/event.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <sstream>

DungeonInter::DungeonInter()
{
    // TODO Get from DungeonData
    m_grid.setRowsColumns(7, 5);

    m_contextMenu.setVisible(false);
}

//-------------------//
//----- Routine -----//

void DungeonInter::init()
{
    core()->add(&m_contextMenu);
    update();
}

void DungeonInter::update()
{
    clearParts();
    addPart(&m_grid);
}

//------------------------//
//----- Mouse events -----//

void DungeonInter::handleMouseEvent(const sf::Event& event, const sf::Vector2f& relPos)
{
    switch (event.type) {
    case sf::Event::MouseButtonPressed:
        handleMousePressed(event, relPos);
        break;
    case sf::Event::MouseMoved:
        handleMouseMoved(event, relPos);
        break;
    case sf::Event::MouseLeft:
        handleMouseLeft();
        break;
    default:
        break;
    }
}

void DungeonInter::handleMousePressed(const sf::Event& event, const sf::Vector2f& relPos)
{
    sf::Vector2f fixPos = getInverseTransform().transformPoint(relPos);

    if (event.mouseButton.button == sf::Mouse::Right) {
        m_selectedRoom = m_grid.rowColumnFromCoords(fixPos);

        std::function<void()> constructRoom = [] { std::cout << "Click -> room constructed!" << std::endl; };

        std::wstringstream roomName;
        roomName << _("Room") << " " << m_selectedRoom.x << " - " << m_selectedRoom.y;

        // Examples of choices
        m_contextMenu.clearChoices();
        m_contextMenu.addChoice(roomName.str());
        m_contextMenu.addChoice(L"Test callback", constructRoom);
        m_contextMenu.addChoice(L"Nothing happens here", nullptr);

        m_contextMenu.setPosition(relPos);
        m_contextMenu.setVisible(true);
        setStatus(true);
    }
    else {
        m_contextMenu.setVisible(false);
    }
}

void DungeonInter::handleMouseMoved(const sf::Event& event, const sf::Vector2f& relPos)
{
}

void DungeonInter::handleMouseLeft()
{
    // Not usable yet...
    // m_contextMenu.setVisible(false);
}

bool DungeonInter::handleKeyboardEvent(const sf::Event& event)
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

//-------------------//
//----- Refresh -----//

void DungeonInter::changedSize()
{
    m_grid.setSize(size());
}

