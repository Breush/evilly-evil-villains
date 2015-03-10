#pragma once

#include "scene/entity.hpp"
#include "nui/contextmenu.hpp"
#include "sfe/grid.hpp"
#include "dungeon/data.hpp"

namespace dungeon
{
    class Inter : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:
        Inter(nui::ContextMenu& contextMenu);
        virtual ~Inter() = default;

        // Events
        void handleGlobalEvent(const sf::Event& event);
        void handleMouseButtonPressed(const sf::Mouse::Button& button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        void handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        void handleMouseLeft() override;

        // Dungeon data
        void useData(Data& data);
        void refreshFromData();

        // Room management
        void refreshRoomTiles();
        void setRoomTile(const uint floor, const uint room, const Data::Room& roomInfo);

        // Global data management
        void adaptFloorsCount(int relativeValue);
        void adaptRoomsByFloor(int relativeValue);
        void setFloorsCount(uint value);
        void setRoomsByFloor(uint value);

        // Ladder
        void constructLadder(const sf::Vector2f& relPos);

    protected:
        // Virtual
        void update() override;

        // Display
        void refreshRoomSelectedShader();
        void setHasRoomSelected(bool hasRoomSelected);

        // Room management
        void switchSelectedRoomState();
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
