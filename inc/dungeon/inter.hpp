#pragma once

#include "scene/entity.hpp"
#include "context/event.hpp"
#include "dungeon/data.hpp"
#include "dungeon/commandable.hpp"
#include "dungeon/elements/trap.hpp"
#include "dungeon/elements/facility.hpp"
#include "nui/contextmenu.hpp"
#include "nui/spinbox.hpp"
#include "sfe/grid.hpp"
#include "scene/wrappers/label.hpp"
#include "scene/wrappers/rectangleshape.hpp"
#include "scene/wrappers/customline.hpp"
#include "tools/vector.hpp"

#include <array>
#include <unordered_map>

namespace dungeon
{
    //! The dungeon itself, handling user interactions.

    class Inter final : public scene::Entity, private context::EventReceiver
    {
        using baseClass = scene::Entity;

        //! A basic void callback.
        using Callback = std::function<void()>;

        //! A moving room.
        struct MovingRoom
        {
            float animationTime = 0.f;              //!< Current time of animation (milliseconds).
            float animationDelay = 0.f;             //!< Total time of animation (milliseconds).
            sf::Vector2u coords;                    //!< Which room to move (starting coords).
            sf::Vector2f velocity;                  //!< At which speed and direction to go.
            Callback onFinishCallback = nullptr;    //!< A function to execute when this animation finished.
        };

    public:

        //! Constructor.
        Inter(nui::ContextMenu& contextMenu);

        //! Default destructor.
        virtual ~Inter() = default;

        //! Initialize after textures are loaded.
        void init();

        std::string _name() const final { return "dungeon::Inter"; }

        //----------------//
        //! @name Events
        //! @{

        //! Should be called whenever an event occurs.
        void handleGlobalEvent(const sf::Event& event);

        //! @}

        //--------------------//
        //! @name Data events
        //! @{

        inline Data& data() { return *m_data; }

        //! Set the dungeon data source.
        void useData(Data& data);

        //! @}

        //------------------------//
        //! @name Dosh harvesting
        //! @{

        //! Harvest the harvetable dosh in the specified tile.
        void harvestTileDosh(const sf::Vector2u& coords);

        //! @}

        //-------------------//
        //! @name Prediction
        //! @{

        //! Remove the current prediction overlay.
        void resetPrediction();

        //! Set the prediction overlay to a monster.
        void setPredictionMonster(const sf::Vector2f& relPos, const std::wstring& monsterID);

        //! Remove the current link prediction.
        void resetPredictionLink();

        //! Show a link prediction between two rooms.
        void setPredictionLink(const sf::Vector2u& coords, const sf::Vector2u& linkCoords);

        //! @}

        //----------------//
        //! @name Control
        //! @{

        //! Computes the correct roomScale from the expected width.
        void setRoomWidth(const float roomWidth);

        //! Get the room scale factors, relative to original image size.
        inline const sf::Vector2f& roomScale() const { return m_roomScale; }

        //! @}

        //------------------//
        //! @name Structure
        //! @{

        //! Find the room below the specified relative position and construct it if possible.
        inline void constructRoom(const sf::Vector2f& relPos) { constructRoom(tileFromLocalPosition(relPos)); }

        //! Construct the room, after cheking against price if not free.
        void constructRoom(const sf::Vector2u& coords, bool free = false);

        //! Find the room below the specified relative position and remove it if any.
        inline void destroyRoom(const sf::Vector2f& relPos) { destroyRoom(tileFromLocalPosition(relPos)); }

        //! Remove the room at coords if any, without gaining money if loss is enabled.
        void destroyRoom(const sf::Vector2u& coords, bool loss = false);

        //! Find the room below the specified relative position and push it.
        inline bool pushRoom(const sf::Vector2f& relPos, Direction direction, uint animationTime)
            { return pushRoom(tileFromLocalPosition(relPos), direction, animationTime); }

        //! Push the room at coords if any.
        //! @return true on success or if no room and false if action is impossible.
        bool pushRoom(const sf::Vector2u& coords, Direction direction, uint animationDelay = 0u);

        //! Change the number of floors.
        void adaptFloorsCount(int relativeValue);

        //! Change the number of rooms by floor.
        void adaptRoomsByFloor(int relativeValue);

        //! Set the number of floors.
        void setFloorsCount(uint value);

        //! Set the number of rooms by floor.
        void setRoomsByFloor(uint value);

        //! @}

        //-------------------//
        //! @name Facilities
        //! @{

        //! Return true if a facility exists in this room.
        bool hasRoomFacility(const sf::Vector2u& coords, const std::wstring& facilityID) const;

        //! How much money do you get back if you're removing the facility in the specified room.
        uint gainRemoveRoomFacility(const sf::Vector2u& coords, const std::wstring& facilityID) const;

        //! How much money do you get back if you're removing all the facilities in the specified room.
        uint gainRemoveRoomFacilities(const sf::Vector2u& coords) const;

        //! Find the room and forward change to data.
        bool createRoomFacility(const sf::Vector2u& coords, const std::wstring& facilityID, bool free = false);

        //! Create a facility, and try to link it to another one.
        bool createRoomFacilityLinked(const sf::Vector2u& coords, const std::wstring& facilityID, const sf::Vector2u& linkCoords, const std::wstring& linkFacilityID, bool free = false);

        //! Set the room facility link to specific coordinates.
        void setRoomFacilityLink(const sf::Vector2u& coords, const std::wstring& facilityID, const sf::Vector2u& linkCoords);

        //! Set the specified room facility's barrier.
        void setRoomFacilityBarrier(const sf::Vector2u& coords, const std::wstring& facilityID, bool activated);

        //! Find the room below the specified relative position and remove all the facilities in it if any.
        inline void removeRoomFacilities(const sf::Vector2f& relPos) { removeRoomFacilities(tileFromLocalPosition(relPos)); }

        //! Remove all the facilities in the room.
        void removeRoomFacilities(const sf::Vector2u& coords, bool loss = false);

        //! Remove a specific facility in the room if possible.
        void removeRoomFacility(const sf::Vector2u& coords, const std::wstring& facilityID, bool loss);

        //! @}

        //--------------//
        //! @name Traps
        //! @{

        //! How much money do you get back if you're removing the trap in the specified room.
        uint gainRemoveRoomTrap(const sf::Vector2u& coords) const;

        //! Find the room below the specified relative position and forward change to data.
        inline void setRoomTrap(const sf::Vector2f& relPos, const std::wstring& trapID) { setRoomTrap(tileFromLocalPosition(relPos), trapID); }

        //! Set the room trap, after cheking against price if not free.
        //! This will remove the previous trap if any.
        void setRoomTrap(const sf::Vector2u& coords, const std::wstring& trapID, bool free = false);

        //! Find the room below the specified relative position and remove the trap in it if any.
        inline void removeRoomTrap(const sf::Vector2f& relPos) { removeRoomTrap(tileFromLocalPosition(relPos)); }

        //! Remove the room trap, without gaining money if loss is enabled.
        void removeRoomTrap(const sf::Vector2u& coords, bool loss = false);

        //! @}

        //-----------------//
        //! @name Monsters
        //! @{

        //! Move a monster from reserve into the dungeon.
        void moveMonsterFromReserve(const sf::Vector2f& relPos, const std::wstring& monsterID);

        //! Add a monster to the reserve.
        void addMonsterToReserve(const std::wstring& monsterID);

        //! @}

        //-----------------//
        //! @name Dynamics
        //! @{

        //! Create a dynamic element.
        //! @return the UID of the element created.
        uint32 spawnDynamic(const sf::Vector2f& relPos, const std::wstring& dynamicID);

        //! @}

        //------------------------//
        //! @name Tile management
        //! @{

        //! Return the coordinates (floor/room) of the room below.
        sf::Vector2u tileFromLocalPosition(const sf::Vector2f& pos) const;

        //! Returns the top-left coordinates in pixels of the tile situated at these coordinates.
        sf::Vector2f tileLocalPosition(const sf::Vector2u& coords) const;

        //! Return the relative coordinates (floor/room) of the room below.
        sf::Vector2f relTileFromLocalPosition(const sf::Vector2f& pos) const;

        //! Returns the top-left coordinates in pixels of the tile situated at these relative coordinates.
        sf::Vector2f relTileLocalPosition(const sf::Vector2f& relCoords) const;

        //! The size in pixels of the tiles.
        sf::Vector2f tileSize() const { return m_grid.cellSize(); }

        //! @}

        //---------------//
        //! @name Access
        //! @{

        //! Get the villain from data.
        inline context::Villain& villain() { return m_data->villain(); }

        //! Get the monsters database from data.
        inline const MonstersDB& monstersDB() const { return m_data->monstersDB(); }

        //! Get the facilities database from data.
        inline const FacilitiesDB& facilitiesDB() const { return m_data->facilitiesDB(); }

        //! Get the traps database from data.
        inline const TrapsDB& trapsDB() const { return m_data->trapsDB(); }

        //! Get the traps database from data.
        inline const HeroesDB& heroesDB() const { return m_data->heroesDB(); }

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

        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos) final;
        bool handleMouseMoved(const sf::Vector2f& relPos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        void receive(const context::Event& event) final;

        //! @}

        //------------------------//
        //! @name Tile management
        //! @{

        //! Remove all layers from the tile.
        void clearLayers(const sf::Vector2u& coords);

        //! Add a layer to the tile.
        void addLayer(const sf::Vector2u& coords,  const std::string& textureID, float depth = 50.f);

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
        //! @name Augmented UI
        //! @{

        //! Pop the context menu for the specified tile coordinates at the position.
        void showTileContextMenu(const sf::Vector2u& coords, const sf::Vector2f& nuiPos);

        //! Edit the treasure dosh in the specified room coordinates.
        void showEditTreasureDialog(const sf::Vector2u& coords);

        //! @}

        //----------------//
        //! @name Decorum
        //! @{

        //! Set the dosh label to a specific value.
        void configureDoshLabel(std::unique_ptr<scene::Label>& doshLabel, const uint dosh, const sf::Color& color);

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Refresh the size using current values for roomScale and room number.
        void refreshSize();

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

        //! Refresh the facilities (as tile entities) of the specified tile.
        void refreshTileFacilities(const sf::Vector2u& coords);

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
            sf::Vector2u coords;                                        //!< The coordinates of the tile (floor/room).
            std::vector<std::unique_ptr<scene::RectangleShape>> layers; //!< All sprites to draw, from furthest to nearest.
            std::vector<std::unique_ptr<Facility>> facilities;          //!< The facilities in the tile.
            std::unique_ptr<Trap> trap = nullptr;                       //!< The trap, protecting the tile.
            std::unique_ptr<scene::Label> totalDoshLabel;               //!< The total dosh inside the room.
            std::unique_ptr<scene::Label> harvestableDoshLabel;         //!< The harvestable dosh.
            bool movingLocked = false;                                  //!< Is the tile locked because it is moving?
        };

    private:

        Data* m_data = nullptr;                 //!< Dungeon data.
        Commandable m_commandable;              //!< The interpreter.

        // Display
        sfe::Grid m_grid;                                   //!< The internal grid for overlay display.
        std::unordered_map<sf::Vector2u, Tile> m_tiles;     //!< All tiles constituing the dungeon.
        sf::Vector2f m_roomScale = {1.f, 1.f};              //!< The room scale.
        sf::Vector2f m_refRoomSize;                         //!< The original room size.

        // Decorum
        std::array<sf::RectangleShape, 2u> m_outerWalls;    //!< Sprites for left/right outer walls.
        sf::RectangleShape m_voidBackground;                //!< The void background of each room.

        // Animation
        std::vector<MovingRoom> m_movingRooms;      //!< The rooms to animate.

        // Prediction
        std::wstring m_predictionID;                //!< The current ID of the element overlay.
        scene::AnimatedSprite m_predictionSprite;   //!< The current sprite shown.
        scene::CustomLine m_predictionLink;         //!< The current link shown.

        // NUI
        nui::ContextMenu& m_contextMenu;            //!< The context menu, got from global state.
        nui::SpinBox<uint32> m_treasureEditSpinBox; //!< The spinbox for treasure edition.

        // Tiles
        Tile* m_hoveredTile = nullptr;  //!< If a tile is hovered, this is it.
        Tile* m_selectedTile = nullptr; //!< If a tile is selected, this is it.
    };
}
