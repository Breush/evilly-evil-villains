#pragma once

#include "nui/object.hpp"
#include "nui/contextmenu.hpp"
#include "drawables/grid.hpp"
#include "dungeon/data.hpp"

namespace dungeon
{
    class Inter : public nui::Object
    {
        typedef nui::Object baseClass;

    public:
        Inter();
        virtual ~Inter() {}

        // Virtual
        void init() override;
        void update() override;
        void handleMouseEvent(const sf::Event& event, const sf::Vector2f& relPos) override;
        bool handleKeyboardEvent(const sf::Event& event) override;
        //bool handleJoystickEvent(const sf::Event& event) override;

        // Dungeon data
        void useData(Data& data);
        void refreshFromData();

        // Room management
        void refreshRoomTiles();
        void setRoomTileState(const uint floor, const uint room, const Data::RoomState state);

    protected:
        // Mouse events
        void handleMousePressed(const sf::Event& event, const sf::Vector2f& relPos);
        void handleMouseMoved(const sf::Event& event, const sf::Vector2f& relPos);
        void handleMouseLeft();

        void changedSize() override;

    private:
        // Display
        sfe::Grid m_grid;
        nui::ContextMenu m_contextMenu;
        std::vector<std::vector<sf::RectangleShape>> m_roomTiles;

        // Internal
        sf::Vector2u m_selectedRoom;
        Data* m_data;
    };
}
