#pragma once

#include "scene/entity.hpp"
#include "nui/contextmenu.hpp"
#include "sfe/grid.hpp"
#include "dungeon/data.hpp"

namespace dungeon
{
    class Inter : public scene::Entity
    {
        typedef scene::Entity baseClass;

    public:
        Inter();
        virtual ~Inter() {}

        // Events
        void handleGlobalEvent(const sf::Event&) override;
        void handleKeyboardEvent(const sf::Event& event) override;
        void handleMouseButtonPressed(const sf::Mouse::Button& mouseButton, const sf::Vector2f& mousePos) override;
        void handleMouseMoved(const sf::Vector2f& mousePos) override;
        void handleMouseLeft() override;

        // Dungeon data
        void useData(Data& data);
        void refreshFromData();

        // Room management
        void refreshRoomTiles();
        void setRoomTileState(const uint floor, const uint room, const Data::RoomState state);

        // Global data management
        void adaptFloorsCount(int relativeValue);
        void adaptRoomsByFloor(int relativeValue);
        void setFloorsCount(uint value);
        void setRoomsByFloor(uint value);

    protected:
        // Virtual
        void update() override;

        // Room management
        void switchSelectedRoomState();
        sf::Vector2u& selectRoomFromCoords(const sf::Vector2f& coords);

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
