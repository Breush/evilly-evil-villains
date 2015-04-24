#pragma once

#include "scene/entity.hpp"
#include "dungeon/event.hpp"
#include "dungeon/data.hpp"
#include "nui/contextmenu.hpp"
#include "sfe/grid.hpp"
#include "tools/vector.hpp"

#include <unordered_map>

// Forward declarations

enum class TextureID : uint8;

namespace dungeon
{
    //! The dungeon itself, handling user interactions.

    class Inter final : public scene::Entity, private EventReceiver
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        Inter(nui::ContextMenu& contextMenu);

        //! Default destructor.
        virtual ~Inter() = default;

        //----------------//
        //! @name Events
        //! @{

        //! Should be called whenever an event occurs.
        void handleGlobalEvent(const sf::Event& event);

        //! @}

        //--------------------//
        //! @name Data events
        //! @{

        //! Set the dungeon data source.
        void useData(Data& data);

        //! @}

        //------------------//
        //! @name Structure
        //! @{

        // FIXME These functions should not be here...
        // It's to dungeon::data to manage all that.
        // And to send an event each time a modification occurs.

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

        //! @}

        //------------------------//
        //! @name Tile management
        //! @{

        //! Returns the top-left coordinates in pixels of the tile situated at these coordinates.
        sf::Vector2f tileLocalPosition(const sf::Vector2u& coords) const;

        //! The size in pixels of the tiles.
        sf::Vector2f tileSize() const { return m_grid.cellSize(); }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void update() final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        void handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        void receive(const Event& event) final;

        //! @}

        //------------------------//
        //! @name Tile management
        //! @{

        //! Return the coordinates (floor/room) of the room below.
        sf::Vector2u tileFromLocalPosition(const sf::Vector2f& pos);

        //! Remove all layers from the tile.
        void clearLayers(const sf::Vector2u& coords);

        //! Add a layer to the tile.
        void addLayer(const sf::Vector2u& coords, TextureID);

        //! Remove all tiles.
        void clearTiles();

        //! @}

        //----------------------//
        //! @name Selected tile
        //! @{

        //! Select the tile at the specified local position (if any).
        void selectTile(const sf::Vector2f& pos);

        //! Select the select at specified coordinates.
        void selectTile(const sf::Vector2u& coords);

        //! Deselect the selected tile (and remove all visual effects).
        void deselectTile();

        //! @}

        //---------------------//
        //! @name Hovered tile
        //! @{

        //! Set the currently hovered tile.
        void setHoveredTile(const sf::Vector2u& coords);

        //! Remove all hover effects.
        void resetHoveredTile();

        //! @}

        //---------------------//
        //! @name Context menu
        //! @{

        //! Pop the context menu for the specified tile coordinates at the position.
        void showTileContextMenu(const sf::Vector2u& coords, const sf::Vector2f& nuiPos);

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Refresh display from current data.
        void refreshFromData();

        //! Refresh all tiles, updating state from data.
        void refreshTiles();

        //! Refresh the specified tile, updating state from data.
        void refreshTile(const sf::Vector2u& roomCoords);

        //! Refresh the layers (and their texture) of the specified tile.
        void refreshTileLayers(const sf::Vector2u& roomCoords);

        //! @}

        //! A tile of the dungeon.
        struct Tile
        {
            sf::Vector2u coords;                    //!< The coordinates of the tile (floor/room).
            Data::Room* room = nullptr;             //!< The corresponding room within data.
            std::vector<sf::RectangleShape> layers; //!< All sprites to draw, from furthest to nearest.
        };

    private:

        Data* m_data = nullptr; //!< Dungeon data.

        sfe::Grid m_grid;                                   //!< The internal grid for overlay display.
        std::unordered_map<sf::Vector2u, Tile> m_tiles;     //!< All tiles constituing the dungeon.

        // TODO Is it possible to move it inside completly? (No reference)
        nui::ContextMenu& m_contextMenu;    //!< The context menu.

        Tile* m_hoveredTile = nullptr;  //!< If a tile is hovered, this is it.
        Tile* m_selectedTile = nullptr; //!< If a tile is selected, this is it.
    };
}
