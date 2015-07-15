#pragma once

#include "scene/entity.hpp"
#include "dungeon/event.hpp"
#include "dungeon/data.hpp"
#include "dungeon/traps.hpp"
#include "nui/contextmenu.hpp"
#include "nui/spinbox.hpp"
#include "sfe/grid.hpp"
#include "sfe/label.hpp"
#include "tools/vector.hpp"

#include <array>
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

        //------------------------//
        //! @name Dosh harvesting
        //! @{

        //! Harvest the harvetable dosh in the specified tile.
        void harvestTileDosh(const sf::Vector2u& coords);

        //! @}

        //------------------//
        //! @name Structure
        //! @{

        //! Find the room below the specified relative position and forward change to data.
        void createRoomFacility(const sf::Vector2f& relPos, const std::wstring& facilityID);

        //! Find the room below the specified relative position and forward change to data.
        void setRoomTrap(const sf::Vector2f& relPos, const std::wstring& trapID);

        //! Change the number of floors.
        void adaptFloorsCount(int relativeValue);

        //! Change the number of rooms by floor.
        void adaptRoomsByFloor(int relativeValue);

        //! Set the number of floors.
        void setFloorsCount(uint value);

        //! Set the number of rooms by floor.
        void setRoomsByFloor(uint value);

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

        void updateRoutine(const sf::Time& dt) final;
        void onSizeChanges() final;

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

        //! Add a choice to the context menu, given the state of a facility.
        void addFacilityChoice(const sf::Vector2u& coords, const std::wstring& facilityID, const std::wstring& facilityName);

        //! Pop the context menu for the specified tile coordinates at the position.
        void showTileContextMenu(const sf::Vector2u& coords, const sf::Vector2f& nuiPos);

        //! Edit the treasure dosh in the specified room coordinates.
        void showEditTreasureDialog(const sf::Vector2u& coords);

        //! @}

        //----------------//
        //! @name Decorum
        //! @{

        //! Set the dosh label to a specific value.
        void configureDoshLabel(std::unique_ptr<sfe::Label>& doshLabel, const uint dosh, const sf::Color& color);

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Refresh display from current data.
        void refreshFromData();

        //! Refresh all tiles, updating state from data.
        void refreshTiles();

        //! Refresh the specified tile, updating state from data.
        void refreshTile(const sf::Vector2u& coords);

        //! Refresh the layers of the existing neighbours, updating state from data.
        void refreshNeighboursLayers(const sf::Vector2u& coords);

        //! Refresh the layers (and their texture) of the specified tile.
        void refreshTileLayers(const sf::Vector2u& coords);

        //! Refresh the traps (as tile entities) of the specified tile.
        void refreshTileTraps(const sf::Vector2u& coords);

        //! Refresh the dosh label (value and position) of the specified tile.
        void refreshTileDoshLabel(const sf::Vector2u& coords);

        //! Refresh the outer walls size + position.
        void refreshOuterWalls();

        //! @}

        //! A tile of the dungeon.
        struct Tile
        {
            sf::Vector2u coords;                                //!< The coordinates of the tile (floor/room).
            Data::Room* room = nullptr;                         //!< The corresponding room within data.
            std::vector<sf::RectangleShape> layers;             //!< All sprites to draw, from furthest to nearest.
            std::unique_ptr<Trap> trap = nullptr;               //!< The trap, protecting the tile.
            std::unique_ptr<sfe::Label> totalDoshLabel;         //!< The total dosh inside the room.
            std::unique_ptr<sfe::Label> harvestableDoshLabel;   //!< The harvestable dosh.
        };

    private:

        Data* m_data = nullptr; //!< Dungeon data.

        // Display
        sfe::Grid m_grid;                                   //!< The internal grid for overlay display.
        std::unordered_map<sf::Vector2u, Tile> m_tiles;     //!< All tiles constituing the dungeon.
        std::array<sf::RectangleShape, 2u> m_outerWalls;    //!< Sprites for left/right outer walls.

        // Delay
        std::vector<std::function<void()>> m_tileRefreshPending;    //! Pending list of tile refreshTileXXX.
        bool m_invasion = false;        //!< Whether or not we are in invasion mode.

        // NUI
        nui::ContextMenu& m_contextMenu;            //!< The context menu, got from global state.
        nui::SpinBox<uint32> m_treasureEditSpinBox; //!< The spinbox for treasure edition.

        // Tiles
        Tile* m_hoveredTile = nullptr;  //!< If a tile is hovered, this is it.
        Tile* m_selectedTile = nullptr; //!< If a tile is selected, this is it.
    };
}
