#pragma once

#include "scene/entity.hpp"
#include "dungeon/event.hpp"
#include "dungeon/data.hpp"
#include "nui/contextmenu.hpp"
#include "sfe/grid.hpp"

namespace dungeon
{
    class Inter : public scene::Entity, private EventReceiver
    {
        using baseClass = scene::Entity;

    public:
        Inter(nui::ContextMenu& contextMenu);
        virtual ~Inter() = default;

        // Events
        void handleGlobalEvent(const sf::Event& event);
        void handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        void handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        void handleMouseLeft() override;

        // Dungeon data
        void useData(Data& data);
        void refreshFromData();

        // Room management
        void refreshRoomTile(const sf::Vector2u& roomCoords);
        void refreshRoomTiles();
        void refreshRoomTileTexture(const sf::Vector2u& roomCoords);
        sf::Vector2f roomLocalPosition(const sf::Vector2u& room) const;
        sf::Vector2f roomSize() const;

        // Global data management
        void adaptFloorsCount(int relativeValue);
        void adaptRoomsByFloor(int relativeValue);
        void setFloorsCount(uint value);
        void setRoomsByFloor(uint value);

        // Facilities
        void constructLadder(const sf::Vector2f& relPos);
        void constructDoor(const sf::Vector2f& relPos);

        // Rooms
        void constructRoom(const sf::Vector2u& room);
        void destroyRoom(const sf::Vector2u& room);

    protected:
        // Virtual
        void update() final;

        // Data events
        void receive(const Event& event) final;

        // Context menu
        void showRoomContextMenu(const sf::Vector2u& room, const sf::Vector2f& nuiPos);

        // Display
        void refreshRoomSelectedShader();
        void setHasRoomSelected(bool hasRoomSelected);

        // Room management
        sf::Vector2u roomFromCoords(const sf::Vector2f& coords);
        void selectRoomFromCoords(const sf::Vector2f& coords);

    private:
        // Display
        sfe::Grid m_grid;
        nui::ContextMenu& m_contextMenu;
        std::vector<std::vector<sf::RectangleShape>> m_roomTiles;

        // Internal
        bool m_hasRoomSelected = false;
        sf::Vector2u m_selectedRoom;
        Data* m_data = nullptr;
    };
}
